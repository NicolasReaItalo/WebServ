/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_send.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 12:13:52 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/06 17:31:17 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <ctime>
#include <sstream>
#include <fstream>
#include <map>

std::string Server::get_mime_type(const std::string &path) 
{
    
    std::string::size_type idx = path.rfind('.');
    if (idx != std::string::npos) {
        std::string extension = path.substr(idx);
        if (mimeList.find(extension) != mimeList.end())
            return mimeList[extension];
        else return "";
    }
    return "application/octet-stream"; // Default for binary data
}

bool Server::send_header(int client_fd, int size, std::string path)
{
    std::string mime = get_mime_type(path);
        
    std::time_t clock = std::time(NULL);
    std::string time_str = std::ctime(&clock);
            time_str.erase(time_str.find_last_not_of("\n") + 1);
    std::stringstream ss;
    ss  << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: " << mime << "\r\n"
        << "Content-Length: " << size << "\r\n"
        << "time: " << time_str << "\r\n" << "\r\n";
    std::string header = ss.str();
    send(client_fd, header.c_str(), header.size(), 0);
    return true;
}


void Server::send_error(int error_code, int client_fd)
{
    std::time_t clock = std::time(NULL);
        std::string time_str = std::ctime(&clock);
        time_str.erase(time_str.find_last_not_of("\n") + 1);
    std::stringstream link;
    link << "./" << error_code << ".html";
    std::vector<unsigned char> file_content = load_file(link.str());
    
    std::stringstream not_found_response; 
    not_found_response << "HTTP/1.1 " << error_code << ": " << errorCodeList.find(error_code)->second << "\r\n"
            << "Content-Type: text/html\r\n"
            << "Content-Length: " << file_content.size() << "\r\n"
            << "time: " << time_str << "\r\n"
            "\r\n";
    std::string nfr = not_found_response.str();

        send(client_fd, nfr.c_str(), nfr.size(), 0);
        send(client_fd, reinterpret_cast<const char*>(file_content.data()), file_content.size(), 0);
}
