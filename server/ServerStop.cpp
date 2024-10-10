/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerStop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:55 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/10 14:41:13 by qgiraux          ###   ########.fr       */
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

void Server::killAllChildren()
{
    std::map<int, header_infos>::iterator it;
    for(it = cgiList.begin(); it != cgiList.end(); it++)
    {
        if (kill(it->second.cgi_pid, SIGINT) == 0)
        {
            std::ostringstream oss;
            oss << "[serverRun] Killed CGI process with PID " << it->second.cgi_pid << " due to timeout";
            webservLogger.log(LVL_INFO, oss);
        }
    }
    
    
}

 void Server::ServerClose()
 {
    closeAllFd();
    killAllChildren();
    std::map<int, header_infos>::iterator it;
    
    for (it = chunk.begin(); it != chunk.end(); ++it)
    {
        close(it->second.fd_ressource);        
    }
    chunk.clear();
 }

 Server::~Server() {
    closeAllFd();
    ServerClose();
}