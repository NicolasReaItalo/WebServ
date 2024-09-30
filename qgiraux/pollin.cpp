/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollin.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:44 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/30 12:31:02 by qgiraux          ###   ########.fr       */
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
    while (true) {
        bytesRead = recv(fd, buffer, maxBodySize - 1, 0);
        //if receiving a chunk from a chunked POST
        if (bytesRead < 0) 
        {
            if (chunk.find(fd) != chunk.end())
            {
                chunked_post(fd, (char *)&body[0], i, header);
                return;
            }
            {
                std::ostringstream oss;
                oss << "[POLLIN] body from fd " << fd << " fully received, selecting method...";
                webservLogger.log(LVL_DEBUG, oss);
            }
            switch (header.toDo)
                {
                    case POST:
                        method_post(header, body, fd, i);
                    case GET:
                        method_get(header, fd, i);
                        return;
                    case DELETE:
                        method_delete(header, fd, i);
                        return;
                    case AUTOINDEX:
                        method_autoindex(header, fd, i);
                        return;
					case ERROR:
                        method_error(header, fd, i);
					/*if header.ressourcepath.empty() sendError() else GET*/
						return;
                    default:
                        std::ostringstream oss;
                        oss << "[POLLIN] Unknown method on fd " << fd;
                        webservLogger.log(LVL_ERROR, oss);
                        return;
                        
                }
        }
        if (bytesRead == 0)
        {
            // End of file, client disconnected
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
                headerStr = tmp.substr(0, headerEndPos);

                // Convert remaining part to the body (binary-safe)
                std::vector<unsigned char> bodyPart(buffer + headerEndPos + 4, buffer + bytesRead);
                body.insert(body.end(), bodyPart.begin(), bodyPart.end());

                headerParsed = true; // Mark header as parsed
                {
                    std::ostringstream oss;
                    oss << "[POLLIN] header from fd " << fd << " fully received, requesting parsing";
                    webservLogger.log(LVL_DEBUG, oss);
                }
                // Parse header
                header = headerParser(headerStr, std::make_pair(fd_set[fd].address, fd_set[fd].port));                                
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