/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_post.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:40 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/19 12:49:41 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::method_post(header_infos header, std::string body, int fd, int i)
{
    header.fd_ressource = open(header.ressourcePath.c_str(), O_RDWR | O_CREAT);
    // fd_set.insert(header.fd_ressource);
    std::cout << "test " << header.fd_ressource << "\n" ;
    fd_set[header.fd_ressource] = std::make_pair("0", "0");
            if (header.chunked == false)
            {
                if (body.size() >= maxBodySize)//(body.size() != header.bodySize)
                {
                                                        //return error 400 bad request
                    std::cout << "test2-fail\n";
                    sendError(400, fd, header);
                    return ;
                }
                std::cout << "test2-pass\n";
                write(header.fd_ressource, body.c_str(), body.length());
                                                        //return 201 created
                sendError(201, fd, header);
            }
            else
            {
                chunk[fd] = header;
                receive_data(fd, i);
            }
}