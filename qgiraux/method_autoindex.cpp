/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_autoindex.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:42:10 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/27 12:39:47 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <ctime>

void Server::method_autoindex(const header_infos& header, int fd, int i)
{
    std::map<std::string, std::string> index;
    (void)i;
    std::cout << header.uri << " " << header.locationIndex << std::endl;
    std::string root = header.configServer->getFullPath(header.uri, header.locationIndex);
    std::string path = header.ressourcePath;
    

    DIR *currentdir = opendir(path.c_str());
    if (!currentdir) 
    {
        
        std::cout << "can't open directory" << std::endl;
        return;
    }
    for (struct dirent *dent; (dent = readdir(currentdir)) != NULL;)
    {
        const char *file = dent->d_name;
        std::cout << file << std::endl;
        //skip parent and current directory
        if (strcmp(file, ".") == 0 && strcmp(file, "..") != 0)
            continue;
            
        struct stat file_stats;
        std::string ptfile = path + "/" + file;
        if (stat(ptfile.c_str(), &file_stats) == -1)
        {
            std::cout << "error on stat" << std::endl;
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

    ss << "HTTP/1.1 200 OK\r\n"
    << "Date : " << time_str << "\r\nContent-Type : text/html\r\n\r\n";
    std::string body;
	(void)fd;
    // body = header... custom error page path
    if (body.empty())
    {   
        body = generate_index_page(index, header);
    }
    std::cout << body;
    std::string head = ss.str();
    if (-1 == send(fd, head.c_str(), head.size(), 0))
        std::cout << "error sending header\n";

    if (-1 == send(fd, body.c_str(), body.size(), 0))
        std::cout << "error sending body\n";
    if (shutdown(fd, SHUT_WR) == -1) {
        return;
    }
}