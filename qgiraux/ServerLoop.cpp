/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 16:47:51 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/03 11:03:13 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/wait.h> 

static int getFileSize(const char* filename) {
    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return -1;
    }

    // Get the size of the file
    std::streampos fileSize = file.tellg();

    // Close the file
    file.close();

    return static_cast<int>(fileSize);
}

void Server::ServerLoop()
{
    // checking all open fds
    std::map<int, fdsets>::iterator it;
    for (it = fd_set.begin(); it != fd_set.end();)
    {
        if (!it->second.listener && time - it->second.timer > TIMEOUT)
        {
            int tmp = it->first;
            ++it;
            if (fd_set[tmp].client)
            {
                std::ostringstream oss;
                oss << "[serverRun] connexion timeout on fd " << tmp ;
                webservLogger.log(LVL_INFO, oss);
            }
            close(tmp);
            fd_set.erase(tmp);
        }
        else
            ++it;
    }


    //checking CGI in progress
    
}