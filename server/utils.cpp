/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:50:06 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/17 12:37:09 by nrea             ###   ########.fr       */
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
    file_data.push_back('\0');
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

void Server::sendCustomError(header_infos header, int errcode, int fd)
{
    {
        std::ostringstream oss;
        oss << "[custom error] starting for fd " << fd;
        webservLogger.log(LVL_INFO, oss);
    }

    int tr = open(header.ressourcePath.c_str(), O_RDONLY);
    if (tr == -1)
    {
        std::ostringstream oss;
        oss << "[custom error] failed to open " << header.ressourcePath << ", sending 404 to " << fd;
        webservLogger.log(LVL_ERROR, oss);
        sendError(header, 404, fd);
        return;
    }
    close(tr);

    if (header.bodySize > CHUNK_SIZE)
    {
        std::ostringstream oss;
        oss << "[custom error] file bigger than max authorized size, sending by CHUNKS";
        webservLogger.log(LVL_INFO, oss);
        send_chunk(fd, header);
        return;
    }

    std::vector<unsigned char> data = load_file(header.ressourcePath);

    std::string time_str = std::ctime(&time);
    time_str.erase(time_str.find_last_not_of("\n") + 1);
    std::stringstream ss;
    ss << "HTTP/1.1 " << errcode << " " << errorList[errcode] << "\r\n"
       << "Content-Type: " << header.contentType << "\r\n"
       << "Content-Length: " << data.size() << "\r\n"
       << "Date: " << time_str << "\r\n\r\n";

    std::string head = ss.str();
    if (fcntl(fd, F_GETFD) != -1)
    {
        std::ostringstream oss;
        oss << "[custom error] Sending header " << errcode << " " << errorList[errcode] << " to " << fd << "...";
        webservLogger.log(LVL_INFO, oss);

        if (send(fd, head.c_str(), head.size(), MSG_NOSIGNAL | MSG_DONTWAIT) == -1)
        {
            std::ostringstream oss;
            oss << "[custom error] Failed to send header to " << fd;
            webservLogger.log(LVL_ERROR, oss);
        }

        oss.str("");
        oss << "[custom error] Sending file to " << fd << "...";
        webservLogger.log(LVL_INFO, oss);

        if (send(fd, &(data[0]), data.size(), MSG_NOSIGNAL | MSG_DONTWAIT) == -1)
        {
            std::ostringstream oss;
            oss << "[custom error] Failed to send body to " << fd;
            webservLogger.log(LVL_ERROR, oss);
        }
    }

    if (!header.keepAlive)
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
        {
            std::ostringstream oss;
            oss << "Failed to remove fd from epoll: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
        }
        close(fd);
        fd_set.erase(fd);
    }
}

/*prototype for sendError function*/
void Server::sendError(header_infos header, int errcode, int fd)
{
    std::string time_str = std::ctime(&time);
    time_str.erase(time_str.find_last_not_of("\n") + 1);

    std::stringstream ss;
    ss << errcode;
    std::string errorpage = header.configServer->getDirectiveOutput(header.locationIndex, "error_page", ss.str());

    if (!errorpage.empty())
    {
        int tr = open(errorpage.c_str(), O_RDONLY);
        if (tr > 0)
        {
            close(tr);
            header.ressourcePath = errorpage;
            sendCustomError(header, errcode, fd);
            return;
        }
        else
        {
            perror("SendError: Error opening error page");
        }
    }

    ss.str("");
    ss << "HTTP/1.1 " << errcode << " " << errorList[errcode] << "\r\n"
       << "Date: " << time_str << "\r\n"
       << "Content-Type: text/html\r\n\r\n";

    std::string body = generate_error_page(errcode);
    std::string head = ss.str();

    if (send(fd, head.c_str(), head.size(), MSG_NOSIGNAL | MSG_DONTWAIT) == -1)
    {
        perror("SendError: Failed to send header");
    }

    if (send(fd, body.c_str(), body.size(), MSG_NOSIGNAL | MSG_DONTWAIT) == -1)
    {
        perror("SendError: Failed to send body");
    }

    if (shutdown(fd, SHUT_WR) == -1)
    {
        perror("shutdown");
    }
}

/*prototype for sendError function*/
void Server::sendError(int errcode, int fd)
{
    std::string time_str = std::ctime(&time);
    time_str.erase(time_str.find_last_not_of("\n") + 1);

    std::stringstream ss;
    ss.str("");
    ss << "HTTP/1.1 " << errcode << " " << errorList[errcode] << "\r\n"
       << "Date: " << time_str << "\r\n"
       << "Content-Type: text/html\r\n\r\n";

    std::string body = generate_error_page(errcode);
    std::string head = ss.str();

    if (send(fd, head.c_str(), head.size(), MSG_NOSIGNAL | MSG_DONTWAIT) == -1)
    {
        perror("SendError: Failed to send header");
    }

    if (send(fd, body.c_str(), body.size(), MSG_NOSIGNAL | MSG_DONTWAIT) == -1)
    {
        perror("SendError: Failed to send body");
    }

    if (shutdown(fd, SHUT_WR) == -1)
    {
        perror("shutdown");
    }
}





void Server::failed_to_send(int fd)
{
    std::ostringstream oss;
    oss << "[failed_to_send] Failed to send data: " << strerror(errno);
    webservLogger.log(LVL_ERROR, oss);
    close(fd);
    fd_set.erase(fd);
    chunk.erase(fd);
    return;
}


void removeHeader(const std::string& filename)
{
    std::ifstream inFile(filename.c_str());

    if (!inFile) {
        std::ostringstream oss;
        oss << "[removeHeader] Could not open the file for reading: " << filename;
        webservLogger.log(LVL_ERROR, oss);
        return;
    }

    // Temporary file to store the content after removing the HTTP header
    std::string tempFilename = filename + ".tmp";
    std::ofstream outFile(tempFilename.c_str());

    if (!outFile) {
        std::ostringstream oss;
        oss << "[removeHeader] Could not open the file for writing: " << tempFilename;
        webservLogger.log(LVL_ERROR, oss);
        inFile.close();
        return;
    }

    std::string line;
    bool headerEnded = false;

    // Skip lines until the end of the HTTP header is found
    while (std::getline(inFile, line)) {
        if (line == "\r" || line == "") {
            headerEnded = true;
            break;
        }
    }

    // Write the remaining lines to the temporary file
    while (std::getline(inFile, line)) {
        outFile << line << std::endl;
    }

    inFile.close();
    outFile.close();

    // Now, replace the original file with the temporary file
    if (remove(filename.c_str()) != 0) {
        std::ostringstream oss;
        oss << "[removeHeader] Error deleting the original file: " << filename;
        webservLogger.log(LVL_ERROR, oss);
    } else if (rename(tempFilename.c_str(), filename.c_str()) != 0) {
        std::ostringstream oss;
        oss << "[removeHeader] Error renaming the temporary file: " << tempFilename;
        webservLogger.log(LVL_ERROR, oss);
    }
}

int Server::parse_cgi_tmp_file(header_infos& header)
{
    int tr = open(header.ressourcePath.c_str(), O_RDWR);
    if (tr == -1)
    {
        std::ostringstream oss;
        oss << "[cgi file parser] failed to open cgi tmp file";
        webservLogger.log(LVL_ERROR, oss);
        return 1;
    }
    std::stringstream headerLine;
    char buffer[3];
    size_t pos = headerLine.str().find("\r\n\r\n");
    off_t bytesRead = 0;
    while( pos == std::string::npos)
    {

        int t = read(tr, buffer, 2);
        if (t == 0)
        {
            std::ostringstream oss;
            oss << "[cgi file parser] cgi tmp file format is incorrect";
            webservLogger.log(LVL_ERROR, oss);
            close(tr);
            return 1;
        }
        if (t == -1)
        {
            std::ostringstream oss;
            oss << "[cgi file parser] read error on cgi tmp file";
            webservLogger.log(LVL_ERROR, oss);
            close(tr);
            return 1;
        }
        buffer[t] = '\0';
        headerLine << buffer;
        pos = headerLine.str().find("\r\n\r\n");
        bytesRead += t;
    }
    std::string headers = headerLine.str();
    std::istringstream headerStream(headers);
    std::string line;
    while (std::getline(headerStream, line) && line != "\r")
    {
        if (line.find("Content-Type:") != std::string::npos)
        {
            header.contentType = line.substr(line.find(":") + 2, line.size() - line.find(":") - 3); // Extract content type value
        }
        else if (line.find("Set-Cookie:") != std::string::npos)
        {
            header.cookie = line.substr(line.find(":") + 2, line.size() - line.find(":") - 3); // Extract content type value
        }
    }
    close(tr);

    removeHeader(header.uri);
    return 0;
}
