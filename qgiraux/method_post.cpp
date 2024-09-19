/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_post.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:40 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/19 16:33:40 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::method_post(header_infos header, std::string body, int fd, int i)
{
    header.fd_ressource = open(header.ressourcePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    // fd_set.insert(header.fd_ressource);
    if (-1 == header.fd_ressource)
    {
        std::cout << "failed to open " << header.ressourcePath << std::endl;
        sendError(500, fd, header);
        return;
    }

    else
    
    std::cout << "managed to open  " << header.ressourcePath << std::endl;
    fd_set[header.fd_ressource] = std::make_pair("0", "0");
    if (header.chunked == false)
    {
        if (body.size() >= maxBodySize)//(body.size() != header.bodySize)
        {
                                                //return error 400 bad request
            sendError(413, fd, header);
            return ;
        }
        int bytes_written = write(header.fd_ressource, body.c_str(), body.length());
        if (bytes_written == -1) {
            std::cerr << "Failed to write to the resource: " << strerror(errno) << std::endl;
            sendError(500, fd, header); // Internal Server Error
            return;
        }
        sendError(201, fd, header);
    }
    else
    {
        chunk[fd] = header;
        receive_data(fd, i);
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