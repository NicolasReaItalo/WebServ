/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 16:47:51 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/02 16:58:24 by qgiraux          ###   ########.fr       */
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
    std::map<int, header_infos>::iterator ito;
    for (ito = cgiList.begin(); ito != cgiList.end();)
    {
        // {
        //     std::ostringstream oss;
        //     oss << "[serverRun] checking on cgi fd " << ito->second.cgi_pid << "\n fd timestamp is " << ito->second.timestamp
        //     << "\nserver timestamp is " << time;
        //     webservLogger.log(LVL_INFO, oss);
        // }
        int status;
        int pidStat = waitpid(ito->second.cgi_pid, &status, WNOHANG);
        if (pidStat == -1)
            perror("error: ");
        int tmp = ito->first;
        if (pidStat != 0 && pidStat != -1)
        {
            cgiList[tmp].ressourcePath = cgiList[tmp].uri;
            if (parse_cgi_tmp_file(cgiList[tmp]) != 0)
            {
                sendError(cgiList[tmp], 404, tmp, 0);
            }
            else
            {
                ito->second.bodySize = getFileSize(ito->second.uri.c_str());
                method_get(cgiList[tmp], tmp, 0);
            }
            remove(ito->second.uri.c_str());
            cgiList.erase(tmp);
            ito = cgiList.begin(); // Reset iterator after erase
        
        }
        else if (time - cgiList[tmp].timestamp > TIMEOUT)
        {
            if (kill(ito->second.cgi_pid, SIGINT) == 0)
            {
                std::ostringstream oss;
                oss << "[serverRun] Killed CGI process with PID " << ito->second.cgi_pid << " due to timeout";
                webservLogger.log(LVL_INFO, oss);
            }
            else
            {
                std::ostringstream oss;
                oss << "[serverRun] Failed to kill CGI process with PID " << ito->second.cgi_pid;
                webservLogger.log(LVL_ERROR, oss);
            }
            remove(ito->second.uri.c_str());
            cgiList.erase(tmp);
            ito = cgiList.begin(); // Reset iterator after erase
        }
        else
        {
            ++ito;
        }
    }
}