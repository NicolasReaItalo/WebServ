/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_get.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:38 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/14 10:42:13 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <fcntl.h>
#include <ctime>
#include <sstream>

void Server::method_get(const header_infos& header, int fd)
{
    {
        std::ostringstream oss;
        oss << "[method get] starting for fd " << fd;
        webservLogger.log(LVL_INFO, oss);
    }
    if (header.bodySize > maxBodySize)
    {
        std::ostringstream oss;
        oss << "[method get] file bigger than max authorized size, sending by CHUNKS";
        webservLogger.log(LVL_INFO, oss);
        send_chunk(fd, header);
        return ;
    }
    else
    {
        std::vector<unsigned char> data = load_file(header.ressourcePath);

        char buffer[maxBodySize];
        for (unsigned long i = 0; i < maxBodySize; i++)
            buffer[i] = 0;

        std::string time_str = std::ctime(&time);;
        time_str.erase(time_str.find_last_not_of("\n") + 1);
        std::stringstream ss;
        ss  << "HTTP/1.1 " << header.returnCode << " OK\r\n"
        << "Content-Type: " << header.contentType << "\r\n";
        if (!header.cookie.empty())
            ss << "Set-Cookie: " << header.cookie << "\r\n";
        if (header.bodySize != 0)
            ss << "Content-Length: " << header.bodySize << "\r\n"
            << "time: " << time_str << "\r\n" << "\r\n";
    
        std::string head = ss.str();
        if (fcntl(fd, F_GETFD) != -1)
        {
            {
                std::ostringstream oss;
                oss << "[method get] Sending header 200 -OK to " << fd << "..." ;
                webservLogger.log(LVL_INFO, oss);   
            }
            if (!is_socket_open(fd) || -1 == send(fd, head.c_str(), head.size(), 0))
            {
                std::ostringstream oss;
                oss << "[method get] Failed to send header to " << fd;
                webservLogger.log(LVL_INFO, oss);
            }
            {
                std::ostringstream oss;
                oss << "[method get] Sending file to " << fd << "...";
                webservLogger.log(LVL_INFO, oss);   
            }
            if (!is_socket_open(fd) || -1 == send(fd, &(data[0]), data.size(), MSG_NOSIGNAL))
            {
                std::ostringstream oss;
                oss << "[method get] Failed to send body to " << fd;
                webservLogger.log(LVL_INFO, oss);
            }
        }
        if (header.keepAlive == false)
        {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
            {
                std::ostringstream oss;
                oss << "Failed to remove fd from epoll: " << strerror(errno);
                webservLogger.log(LVL_INFO, oss);
            }
            close(fd);
            std::cout << "closing fd " << fd << "method_get line 101\n";
            fd_set.erase(fd);
        }
    }
    {
        std::ostringstream oss;
        oss << "[method get] file sent to " << fd << "...";
        webservLogger.log(LVL_INFO, oss);   
    }
}
