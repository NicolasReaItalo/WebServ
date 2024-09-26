/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_delete.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:35 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/26 16:03:04 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdio>
#include <ctime>
#include <sstream>

void Server::method_delete(header_infos header, int fd)
{
    if (0 == remove(header.ressourcePath.c_str()))
    {
        //struct stat fileInfo;
        std::string time_str = std::ctime(&time);;
        time_str.erase(time_str.find_last_not_of("\n") + 1);
        std::string mime = mimeList[get_mime_type(header.ressourcePath)];

        std::stringstream ss;
        ss  << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: " << mime << "\r\n"
        << "time: " << time_str << "\r\n" << "\r\n";

        std::string head = ss.str();
        send(fd, head.c_str(), head.size(), 0);
    }
    else
    {
        //send error 500 internal server error
        sendError(500, fd);
    }
}