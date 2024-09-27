/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerStop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:55 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/27 12:33:57 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdio>

void Server::closeAllFd()
{
    std::map<int, fdsets>::iterator it;

    for (it = fd_set.begin(); it != fd_set.end(); it++)
        close(it->first);
    fd_set.clear();
}

 void Server::ServerClose()
 {
    closeAllFd();
    std::map<int, header_infos>::iterator it;
    
    for (it = chunk.begin(); it != chunk.end(); ++it)
    {
        close(it->second.fd_ressource);
        remove(it->second.ressourcePath.c_str());
    }
    chunk.clear();
 }

 Server::~Server() {
    closeAllFd();
    ServerClose();
}