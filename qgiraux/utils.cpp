/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:50:06 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/19 16:30:07 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <ctime>
#include <sstream>
#include <fstream>
#include <ctime>



std::vector<unsigned char> Server::load_file(const std::string &filename) 
{
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return std::vector<unsigned char>(); // Return an empty vector
    }
    // std::cout << " opening file " << filename.c_str() << std::endl;
    std::vector<unsigned char> file_data((std::istreambuf_iterator<char>(file)),
                                           std::istreambuf_iterator<char>());
    return file_data;
}

std::string Server::get_mime_type(const std::string &uri) 
{
    
    std::string::size_type idx = uri.rfind('.');
    if (idx != std::string::npos) {
        std::string extension = uri.substr(idx);
        if (mimeList.find(extension) != mimeList.end())
            return mimeList[extension];
        else return "";
    }
    return "application/octet-stream"; // Default for binary data
}

bool Server::is_fd_in_chunklist(int fd)
{
    if (chunk.find(fd) != chunk.end())
        return true;
    return false;
}

void Server::set_mimeList()
{
	mimeList[".html"]   = "text/html";
    mimeList[".htm"]    = "text/html";
    mimeList[".png"]    = "image/png";
    mimeList[".jpg"]    = "image/jpg";
    mimeList[".jpeg"]   = "image/jpeg";
    mimeList[".gif"]    = "image/gif";
    mimeList[".ico"]    = "image/ico";
    mimeList[".css"]    = "text/css";
    mimeList[".js"]     = "application/javascript";
    mimeList[".json"]   = "application/json";
    mimeList[".pdf"]    = "application/pdf";
    mimeList[".css"]    = "text/css";
}

void Server::set_errorList()
{
    errorList[200]  = "OK";
    errorList[201]  = "Created";
    errorList[202]  = "Accepted";

    errorList[301]  = "Moved Permanently";
    errorList[302]  = "Found";
    errorList[308]  = "Permanent Redirect";

    errorList[400]  = "Bad Request";
    errorList[401]  = "Unauthorized";
    errorList[403]  = "Forbidden";
    errorList[404]  = "Not Found";
    errorList[405]  = "Method Not Allowed";
    errorList[406]  = "Not Acceptable";
    errorList[408]  = "Request Timeout";
    errorList[413]  = "Payload Too Large";
    errorList[414]  = "URI Too Long";
    errorList[415]  = "Unsupported Media Type";

    errorList[500]  = "Internal Server Error";
    errorList[501]  = "Not Implemented";
    errorList[502]  = "Bad Gateway";
    errorList[503]  = "Service Unavailable";
    errorList[504]  = "Gateway Timeout";

}
/*prototype for sendError function*/
void Server::sendError(int errcode, int fd, header_infos header)
{
	(void)header;
    std::stringstream ss;
    std::time_t clock = std::time(NULL);
    std::string time_str = std::ctime(&clock);
    time_str.erase(time_str.find_last_not_of("\n") + 1);

    ss << "HTTP/1.1 " << errcode << " " << errorList[errcode] << "\r\n"
    << "Date : " << time_str << "\r\nContent-Type : text/html\r\n\r\n";
    std::string body;
	(void)fd;
    // body = header... custom error page path
    if (body.empty())
    {
        body = generate_error_page(errcode);
    }
    std::string head = ss.str();
    std::cout << "errcode " << errcode << "\n";
    send(fd, head.c_str(), head.size(), 0);

    send(fd, body.c_str(), body.size(), 0);

}

void Server::failed_to_send(int fd)
{
    std::cerr << "Failed to send data: " << strerror(errno) << std::endl;
    close(fd);
    fd_set.erase(fd);
    chunk.erase(fd);
    return;
}