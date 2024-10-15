/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_return.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:38 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/14 13:50:42 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <fcntl.h>
#include <ctime>
#include <sstream>

void Server::method_return(const header_infos& header, int fd)
{
    {
        std::ostringstream oss;
        oss << "[method return] starting for fd " << fd;
        webservLogger.log(LVL_INFO, oss);
    }

    std::string time_str = std::ctime(&time);
    time_str.erase(time_str.find_last_not_of("\n") + 1);
    std::stringstream ss;
    ss  << "HTTP/1.1 302 Found\r\n"
    << "Location: " << header.ressourcePath << "\r\n"
    << "Content-Length: 0\r\n"
    << "time: " << time_str << "\r\n" << "\r\n";

    std::string head = ss.str();
    if (fcntl(fd, F_GETFD) != -1)
    {
        {
            std::ostringstream oss;
            oss << "[method return] Sending header 302 -Found to " << fd << "...";
            webservLogger.log(LVL_INFO, oss);   
        }
        if (-1 == send(fd, head.c_str(), head.size(), MSG_NOSIGNAL | MSG_DONTWAIT))
        {
            std::ostringstream oss;
            oss << "[method return] Failed to send header to " << fd;
            webservLogger.log(LVL_INFO, oss);
        }
    }
    if (header.keepAlive == false)
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
        {
            std::ostringstream oss;
            oss << "[method return] Failed to remove fd from epoll: " << strerror(errno);
            webservLogger.log(LVL_INFO, oss);
        }
        close(fd);
        fd_set.erase(fd);
    }
}
