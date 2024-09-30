/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:50:06 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/30 13:57:31 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <ctime>
#include <sstream>
#include <fstream>
#include <ctime>



std::vector<unsigned char> Server::load_file(const std::string &filename)
{
    {
        std::ostringstream oss;
        oss << "[Load_file] loading file " << filename << "...";
        webservLogger.log(LVL_DEBUG, oss);
    }
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file)
    {
        std::ostringstream oss;
        oss << "[Load_file] Error opening file: " << filename;
        webservLogger.log(LVL_DEBUG, oss);
        return std::vector<unsigned char>(); // Return an empty vector
    }
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
    }
    return "text/plain"; // Default for binary data
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
    errorList[409]  = "Conflict";
    errorList[413]  = "Payload Too Large";
    errorList[414]  = "URI Too Long";
    errorList[415]  = "Unsupported Media Type";

    errorList[500]  = "Internal Server Error";
    errorList[501]  = "Not Implemented";
    errorList[502]  = "Bad Gateway";
    errorList[503]  = "Service Unavailable";
    errorList[504]  = "Gateway Timeout";

}

void Server::sendCustomError(header_infos header, int errcode, int fd, int i)
{
    {
        std::ostringstream oss;
        oss << "[custom error] starting for fd " << fd;
        webservLogger.log(LVL_INFO, oss);
    }
    {
        int tr = open(header.ressourcePath.c_str(), O_RDONLY);
        if (-1 == tr)
        {
            {
            std::ostringstream oss;
            oss << "[custom error] failed to open " << header.ressourcePath << ", sending 404 to "<<fd;
            webservLogger.log(LVL_ERROR, oss);
            }
            sendError(header, 404, fd, i);
            return;
        }
        close(tr);
    }
    if (header.bodySize > CHUNK_SIZE)
    {
        std::ostringstream oss;
        oss << "[custom error] file bigger than max authorized size, sending by CHUNKS";
        webservLogger.log(LVL_INFO, oss);
        send_chunk(fd, i, header);
        return ;
    }
    else
    {
        std::vector<unsigned char> data = load_file(header.ressourcePath);

        char buffer[maxBodySize];
        for (unsigned long i = 0; i < maxBodySize; i++)
            buffer[i] = 0;

        std::string time_str = std::ctime(&time);;
        time_str.erase(time_str.find_last_not_of("\n") + 1);
        std::stringstream ss;
        ss  << "HTTP/1.1 "<< errcode << errorList[errcode] << "\r\n"
        << "Content-Type: " << header.contentType << "\r\n"
        << "Content-Length: " << data.size() << "\r\n"
        << "time: " << time_str << "\r\n" << "\r\n";

        std::string head = ss.str();
        if (fcntl(fd, F_GETFD) != -1)
        {
            {
                std::ostringstream oss;
                oss << "[custom error] Sending header " << errcode << errorList[errcode] << " to " << fd << "...";
                webservLogger.log(LVL_INFO, oss);   
            }
            if (-1 == send(fd, head.c_str(), head.size(), 0))
            {
                std::ostringstream oss;
                oss << "[custom error] Failed to send header to " << fd;
                webservLogger.log(LVL_INFO, oss);
            }
            {
                std::ostringstream oss;
                oss << "[custom error] Sending file to " << fd << "...";
                webservLogger.log(LVL_INFO, oss);   
            }
            if (-1 == send(fd, &(data[0]), header.bodySize, 0))
            {
                std::ostringstream oss;
                oss << "[custom error] Failed to send body to " << fd;
                webservLogger.log(LVL_INFO, oss);
            }
        }
        if (header.keepAlive == false)
        {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
            {
                std::ostringstream oss;
                oss << "Failed to remove fd from epoll: " << strerror(errno);
                webservLogger.log(LVL_INFO, oss);
            }
            close(fd);
            fd_set.erase(fd);
        }
    }
}

/*prototype for sendError function*/
void Server::sendError(header_infos header, int errcode, int fd, int i)
{
    std::string time_str = std::ctime(&time);
    std::string body;
    
    {
        std::stringstream ss;
        ss << errcode;
        std::string errorpage = header.configServer->getDirectiveOutput(header.locationIndex, "error_page", ss.str());
        if (errorpage != "")
        {
            int tr = open(errorpage.c_str(), O_RDONLY);
            if (tr > 0)
            {
                close(tr);
                std::stringstream oss;             
                header.ressourcePath = errorpage;
                sendCustomError(header, errcode, fd, i);
                return ;
                
            }
            perror("Error opening error page");
            
        }
    }
    std::stringstream ss;
    time_str.erase(time_str.find_last_not_of("\n") + 1);

    ss << "HTTP/1.1 " << errcode << " " << errorList[errcode] << "\r\n"
    << "Date : " << time_str << "\r\nContent-Type : text/html\r\n\r\n";
    
	(void)fd;
    // body = header... custom error page path
    if (body.empty())
    {
        body = generate_error_page(errcode);
    }
    std::string head = ss.str();
    send(fd, head.c_str(), head.size(), 0);
    send(fd, body.c_str(), body.size(), 0);

    if (shutdown(fd, SHUT_WR) == -1) {
        perror("shutdown");
        return;
    }
}

void Server::failed_to_send(int fd)
{
    std::cerr << "Failed to send data: " << strerror(errno) << std::endl;
    close(fd);
    fd_set.erase(fd);
    chunk.erase(fd);
    return;
}
