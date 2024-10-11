// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   chunk_receive.cpp                                  :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/10/07 12:25:30 by qgiraux           #+#    #+#             */
// /*   Updated: 2024/10/07 14:59:51 by qgiraux          ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

#include "Server.hpp"
#include <istream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

void Server::chunked_post(int fd, std::vector<unsigned char> body, header_infos& header)
{
    {
        std::ostringstream oss;
        oss << "[method post chunk] starting for a new chunk..." << header.ressourcePath;
        webservLogger.log(LVL_DEBUG, oss);
    }
    fd_set[header.fd_ressource].timer = time;
    std::ostringstream str;
    str << std::string(body.begin(), body.end());

    std::istringstream chunk_stream(str.str());
    std::string chunk_size_str;
    std::vector<unsigned char> parsed_body;

    while (std::getline(chunk_stream, chunk_size_str))
    {
        // Remove any potential carriage return character
        if (!chunk_size_str.empty() && chunk_size_str[chunk_size_str.size() - 1] == '\r')
        {
            chunk_size_str.erase(chunk_size_str.size() - 1);
        }

        // Convert chunk size from hex to integer
        std::istringstream hex_stream(chunk_size_str);
        size_t chunk_size;
        hex_stream >> std::hex >> chunk_size;

        if (chunk_size == 0)
        {
            break; // Last chunk
        }

        // Read the chunk data
        std::vector<unsigned char> chunk_data(chunk_size);
        chunk_stream.read(reinterpret_cast<char*>(&chunk_data[0]), chunk_size);
        parsed_body.insert(parsed_body.end(), chunk_data.begin(), chunk_data.end());
        
        // Skip the CRLF after the chunk data
        chunk_stream.ignore(2);
    }

    // Check if all chunks have been received
    if (parsed_body.size() == 0)
    {
        
        std::string head = "HTTP/1.1 204 No Content\r\n\r\n";
        {
            std::ostringstream oss;
            oss << "[method post] Transferred all the chunks to " << header.ressourcePath;
            webservLogger.log(LVL_DEBUG, oss);
        }
        if (cgiList.find(fd) != cgiList.end())
            method_post_cgi(fd, header);
        else
        {
            if (-1 == send(fd, head.c_str(), head.size(), 0))
                    std::cerr << "error sending header\n";
        }
        close(header.fd_ressource);
        chunk.erase(fd);
        if (!header.keepAlive)
        {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) 
            {
                std::ostringstream oss;
                oss << "Failed to remove fd from epoll: " << strerror(errno);
                webservLogger.log(LVL_ERROR, oss);
            }
            close(fd);
            fd_set.erase(fd); // Remove the file descriptor from the fd_set
        }
        return;
    }

    /*check if size < client_max_body_size*/
    size_t size = std::atol(header.configServer->getDirectiveParameter(header.locationIndex, "client_max_body_size").c_str());
    header.bodySize += parsed_body.size();
    chunk[fd].bodySize = header.bodySize;
    if (header.bodySize > size)
    {
        sendError(header, 413, fd);
        close(header.fd_ressource);
        fd_set.erase(header.fd_ressource);
        close(fd);
        chunk.erase(fd);
        if (!header.keepAlive)
        {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) 
            {
                std::ostringstream oss;
                oss << "Failed to remove fd from epoll: " << strerror(errno);
                webservLogger.log(LVL_ERROR, oss);
            }
            close(fd);
            fd_set.erase(fd); // Remove the file descriptor from the fd_set
        }
        return;
    }

    size_t bytes_written = write(header.fd_ressource, &(parsed_body[0]), parsed_body.size());
    if (bytes_written < 0)
    {
        sendError(header, 500, fd); // Send internal server error if there's a mismatch
        close(header.fd_ressource);
        fd_set.erase(header.fd_ressource);
        close(fd);
        chunk.erase(fd);
        remove(header.ressourcePath.c_str());
        //std::cerr << "remove " << header.ressourcePath.c_str() << "in chunk_receive.cpp line 124" << std::endl;
        if (!header.keepAlive)
        {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) 
            {
                std::ostringstream oss;
                oss << "Failed to remove fd from epoll: " << strerror(errno);
                webservLogger.log(LVL_ERROR, oss);
            }
            close(fd);
            fd_set.erase(fd); // Remove the file descriptor from the fd_set
        }
        return;
    }
}