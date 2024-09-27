/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_get.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:38 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/26 16:03:13 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <fcntl.h>
#include <ctime>
#include <sstream>

void Server::method_get(header_infos header, int fd, int i)
{
    int tr = open(header.ressourcePath.c_str(), O_RDONLY);
    // std::cout << "TR == " << tr << std::endl << "ressource path == " << header.ressourcePath << std::endl;;
    if (-1 == tr)
    {
        sendError(404, fd);
        return;
    }
    close(tr);
    if (header.bodySize > CHUNK_SIZE)
    {
        send_chunk(fd, i, header);
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
        ss  << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: " << header.contentType << "\r\n"
        << "Content-Length: " << header.bodySize << "\r\n"
        << "time: " << time_str << "\r\n" << "\r\n";

        std::string head = ss.str();
        if (fcntl(fd, F_GETFD) != -1)
        {
            if (-1 == send(fd, head.c_str(), head.size(), 0))
                std::cout << "error sending header\n";

            if (-1 == send(fd, &(data[0]), header.bodySize, 0))
                std::cout << "error sending body\n";
        }
        if (header.keepAlive == false)
        {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
            {
                std::cerr << "Failed to remove fd from epoll: " << strerror(errno) << std::endl;
            }
            close(fd);
            fd_set.erase(fd);
        }
    }
}
