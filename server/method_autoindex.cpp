/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_autoindex.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:42:10 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/10 13:10:31 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <ctime>

void Server::method_autoindex(const header_infos& header, int fd)
{
    std::map<std::string, std::string> index;
    std::string root = header.configServer->getFullPath(header.uri, header.locationIndex);
    std::string path = header.ressourcePath;
    
    {
        std::ostringstream oss;
        oss << "[method autoindex] starting for fd " << fd;
        webservLogger.log(LVL_INFO, oss);
    }
    DIR *currentdir = opendir(path.c_str());
    if (!currentdir) 
    {
        std::ostringstream oss;
        oss << "[method autoindex] can't open directory " << currentdir;
        webservLogger.log(LVL_ERROR, oss);
		sendError(header, 403, fd);
        return;
    }
    {
        std::ostringstream oss;
        oss << "[method autoindex] Loading directory content...";
        webservLogger.log(LVL_INFO, oss);
    }
    for (struct dirent *dent; (dent = readdir(currentdir)) != NULL;)
    {
        const char *file = dent->d_name;
        //skip parent and current directory
        if (strcmp(file, ".") == 0 && strcmp(file, "..") != 0)
            continue;
            
        struct stat file_stats;
        std::string ptfile = path + "/" + file;
        if (stat(ptfile.c_str(), &file_stats) == -1)
        {
             std::ostringstream oss;
            oss << "[method autoindex] error on stat for " << ptfile;
            webservLogger.log(LVL_ERROR, oss);
            return;
        }
        std::string filename = file;
        if (dent->d_type == DT_DIR)
            index[filename + "/"] = filename + "/";
        else
            index[filename + ""] = filename;   
    }
    closedir(currentdir);
    send_index(fd, header, index);
    index.clear();
}  

void Server::send_index(int fd, const header_infos& header, std::map<std::string, std::string>& index)
{
    std::stringstream ss;
    std::string time_str = std::ctime(&time);
    time_str.erase(time_str.find_last_not_of("\n") + 1);
    {
        std::ostringstream oss;
        oss << "[method autoindex] sending header 200 -ok to " << fd;
        webservLogger.log(LVL_DEBUG, oss);
    }
    ss << "HTTP/1.1 200 OK\r\n"
    << "Date : " << time_str << "\r\nContent-Type : text/html\r\n\r\n";
    std::string body;
    {
        std::ostringstream oss;
        oss << "[method autoindex] creating autoindex html page ...";
        webservLogger.log(LVL_DEBUG, oss);
    }
    body = generate_index_page(index, header);
    
    std::string head = ss.str();
    {
        std::ostringstream oss;
        oss << "[method autoindex] Sending header to " << fd << "...";
        webservLogger.log(LVL_INFO, oss);   
    }
    if (-1 == send(fd, head.c_str(), head.size(), 0))
    {
        std::ostringstream oss;
        oss << "[method autoindex] error sending header to fd" << fd;
        webservLogger.log(LVL_ERROR, oss);
    }
    {
        std::ostringstream oss;
        oss << "[method autoindex] Sending body to " << fd << "...";
        webservLogger.log(LVL_INFO, oss);   
    }
    if (-1 == send(fd, body.c_str(), body.size(), 0))
    {
        std::ostringstream oss;
        oss << "[method autoindex] error sending body to fd" << fd;
        webservLogger.log(LVL_ERROR, oss);
    }
    if (shutdown(fd, SHUT_WR) == -1) {
        return;
    }
}