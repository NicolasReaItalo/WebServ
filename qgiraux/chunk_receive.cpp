/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chunk_receive.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:15 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/19 12:49:16 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdio>
#include <numeric>

/*extracts the chunk from data, and adds it to the output file
int fd  :           fd on which the data was received
int i   :           index of the server to which the data was sent
std::string data :   data containing the chunk*/
void Server::chunked_post(int fd, std::string data)
{
    /*if the current chunk does not end correctly*/
    if (data.substr(data.length() - 2) != "\r\n")
    {
        if (data.length() < maxBodySize){
                                            //return error 400 bad request
            sendError(400, chunk[fd].fd_ressource, chunk[fd]);
        }
        else{
                                            //return error 413 content to large
            sendError(413, chunk[fd].fd_ressource, chunk[fd]);
        }
        close(chunk[fd].fd_ressource);
        remove(chunk[fd].ressourcePath.c_str());
        if (chunk[fd].keepAlive == false)
        {
            close(fd);
            fd_set.erase(fd);
        }
        fd_set.erase(chunk[fd].fd_ressource);
        chunk.erase(fd);
        return;
    }
    /*last chunk of request body*/
    if (data.substr(0, 3) == "0\r\n")
    {
        if (data == "0\r\n\r\n"){
                                            //send response 201 created
            sendError(201, chunk[fd].fd_ressource, chunk[fd]);
        }
        else{
                                            //return Error 400 bad request
            sendError(400, chunk[fd].fd_ressource, chunk[fd]);
        }
        close(chunk[fd].fd_ressource);
        fd_set.erase(chunk[fd].fd_ressource);
        if (chunk[fd].keepAlive == false)
        {
            close(fd);
            fd_set.erase(fd);
        }
        chunk.erase(fd);
        return ;
    }

    /*not last chunk of the body*/
    int start = data.find("/r/n") + 2;
    unsigned long size = std::atoi(data.substr(0, start - 2).c_str());
    data = data.substr(start, data.length() - 2);
    /*if chunk size incorrect*/

    if (size != data.length())
    {
                                            //return error 400 bad request
        sendError(400, chunk[fd].fd_ressource, chunk[fd]);
        close(chunk[fd].fd_ressource);
        fd_set.erase(chunk[fd].fd_ressource);
        remove(chunk[fd].ressourcePath.c_str());
        if (chunk[fd].keepAlive == false)
        {
            close(fd);
            fd_set.erase(fd);
        }
        chunk.erase(fd);
        return;
    }
    /*the current chunk ends well*/
    write(chunk[fd].fd_ressource, data.c_str(), size);
    return;
}