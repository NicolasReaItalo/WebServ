/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollin.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:44 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/14 14:39:10 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <vector>
#include <sys/socket.h>

/* Reads the data received from the client and either:
    - sends to the correct method or
    - receives and concatenates new chunk
> int fd  : the fd on which the data is received
> int i   : the index of the server to which the data is sent */
void Server::receive_data(int fd, int i)
{
    unsigned char buffer[maxBodySize]; // Buffer to read data (binary-safe)
    ssize_t bytesRead;
    std::vector<unsigned char> body;
    std::string headerStr = ""; // String to hold the header part
    header_infos header;

    bool headerParsed = false; // Flag to track whether the header has been parsed
    
    {
        std::ostringstream oss;
        oss << "[POLLIN] Loading data from fd " << fd;
        webservLogger.log(LVL_DEBUG, oss);
    }
    if (chunk.find(fd) != chunk.end())
    {
        headerParsed = true;
        header = chunk[fd];
    }
    
    while (true) {
        bytesRead = recv(fd, buffer, maxBodySize - 1, MSG_NOSIGNAL | MSG_DONTWAIT);
        
        //if receiving a chunk from a chunked POST
        if (bytesRead < 0) 
        {
            {
                std::ostringstream oss;
                oss << "[POLLIN] body from fd " << fd << " fully received, selecting method..." << header.toDo;
                webservLogger.log(LVL_DEBUG, oss);
            }
            if (chunk.find(fd) != chunk.end() && chunk[fd].timestamp == POST)
            {
                fd_set[fd].timer = time;
                chunked_post(fd,  body, chunk[fd]);
                return;
            }
            // std::cout << "body is " << &(body[0]) << std::endl;
            switch (header.toDo)
            {
                case POST:
                {
                    if (chunk.find(fd) != chunk.end())
                        chunked_post(fd, body, chunk[fd]);
                    else
                        method_post(header, body, fd);
                    return;
                }
                case GET:
                    method_get(header, fd);
                    return;
                case GET_CGI:
                    method_get_cgi(header, fd);
                    return;
                case POST_CGI:
                {
                    cgiList[fd] = header;
                    header.toDo = POST;
                    std::stringstream opath;
	                opath << "/tmp/tmpfilein" << tmpfindex++;
                    header.infile = header.ressourcePath;
                    header.ressourcePath = opath.str();
                    method_post(header, body, fd);
                    return;
                }

                case DELETE:
                    method_delete(header, fd);
                    return;
                case AUTOINDEX:
                    method_autoindex(header, fd);
                    return;
                case ERROR:
                    method_error(header, fd);
                    return;
                case RETURN:
                    method_return(header, fd);
                    return;
                default:
                    std::ostringstream oss;
                    oss << "[POLLIN] Unknown method on fd " << fd << " : " << header.toDo << "\n" << headerStr << std::endl;
                    webservLogger.log(LVL_ERROR, oss);
                    close(fd);
                    if (chunk.find(fd) != chunk.end())
                        chunk.erase(fd);
                    fd_set.erase(fd);
                    if (cgiList.find(fd) != cgiList.end())
                        cgiList.erase(fd);
                    
                    return;  
            }
        }
        // End of file, client disconnected
        if (bytesRead == 0)
        {
            {
                std::ostringstream oss;
                oss << "[POLLIN] Client disconnected : " << fd;
                webservLogger.log(LVL_DEBUG, oss);
            }
            close(fd);
            fd_set.erase(fd);
            return;                
        }

        // If the header is not yet parsed, look for the header/body separation
        if (bytesRead > 0 && !headerParsed && chunk.find(fd) == chunk.end()) 
        {
            std::string tmp(reinterpret_cast<char*>(buffer), bytesRead);

            // Find the position where the header ends (marked by \r\n\r\n)
            size_t headerEndPos = tmp.find("\r\n\r\n");
            
            if (headerEndPos != std::string::npos) 
            {
                // Extract header
                size_t l = headerStr.size();
                headerStr += tmp;
                headerStr= headerStr.substr(0, l+ headerEndPos);

                // Convert remaining part to the body (binary-safe)
                std::vector<unsigned char> bodyPart(buffer + headerEndPos + 4, buffer + bytesRead);
                body.insert(body.end(), bodyPart.begin(), bodyPart.end());

                
                {
                    std::ostringstream oss;
                    oss << "[POLLIN] header from fd " << fd << " fully received, requesting parsing";
                    webservLogger.log(LVL_DEBUG, oss);
                }
                // Parse header
                if (chunk.find(fd) == chunk.end())
                {
                    headerParsed = true; // Mark header as parsed
                    header = headerParser(headerStr, std::make_pair(fd_set[fd].address, fd_set[fd].port));
                }
                else 
                    header = chunk[fd];
                header.i_ev = i;
            } 
            else 
            {
                // The header is incomplete; append to headerStr and continue reading
                headerStr += tmp;
                continue;
            }
        } 
        else 
        {
            // After the header has been parsed, continue receiving the body
            body.insert(body.end(), buffer, buffer + bytesRead);
        }
    }
}
