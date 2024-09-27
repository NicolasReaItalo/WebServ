/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_autoindex.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:42:10 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/26 15:52:46 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <ctime>

void Server::method_autoindex(header_infos header, int fd, int i)
{
    std::list<ConfigServer>::iterator it = servers.begin();
    std::map<std::string, std::string> index;
    
    for (int n = 0; n < i; n++)
        it++;
    std::string root = it->getDirectiveParameter(header.locationIndex, "root");
    std::string path = header.ressourcePath;
    
    if (chdir(path.c_str()) == -1)
    {
        std::cout << "can't open directory" << std::endl;
        return;
    }
    if (strncmp(path.c_str(), root.c_str(), root.length()) && path.length() > root.length())
    {
        if (path.rfind("/") == path.length())
            path = path.substr(0, path.length() - 1);
        path = path.substr(0, path.rfind("/") - 1);
        index["../"] = path;         
    }
    DIR *currentdir = opendir(".");
    if (!currentdir) 
    {
        std::cout << "can't open directory" << std::endl;
        return;
    }
    for (struct dirent *dent; (dent = readdir(currentdir)) != NULL;)
    {
        const char *file = dent->d_name;
        //skip parent and current directory
        if (strcmp(file, ".") == 0 || strcmp(file, "..") == 0)
            continue;
            
        struct stat file_stats;
        if (stat(file, &file_stats) == -1)
        {
            return;
        }
        std::string filename = file;
        if (dent->d_type == DT_DIR)
            index[filename + "/"] = filename + "/";
        else
            index[filename + ""] = filename;   
    }
    send_index(fd, header, index);
}  

void Server::send_index(int fd, header_infos header, std::map<std::string, std::string> index)
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
    std::string head = ss.str();
    send(fd, head.c_str(), head.size(), 0);
    send(fd, body.c_str(), body.size(), 0);
    
    if (shutdown(fd, SHUT_WR) == -1) {
        return;
    }
}