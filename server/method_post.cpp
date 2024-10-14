/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_post.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:40 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/14 12:22:42 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iomanip>
#include "Logger.hpp"
#include <ostream>


void Server::method_post(header_infos& header, std::vector<unsigned char> body, int fd)
{
    {
        std::ostringstream oss;
        oss << "[method post] starting for fd " << fd;
        webservLogger.log(LVL_INFO, oss);
    }
    // Open the target file to write the body data
    header.fd_ressource = open(header.ressourcePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (-1 == header.fd_ressource)
    {
        std::ostringstream oss;
		oss << "[method post] Failed to open file: " << header.ressourcePath;
		webservLogger.log(LVL_ERROR, oss);
        sendError(header, 500, fd); // Send internal server error if file open fails
        return;
    }
    fdsets tmp = {"0","0",time, false, false};
    fd_set[header.fd_ressource] = tmp;

    // If the request is not chunked (i.e., receiving the entire body in one go)
    if (!header.chunked)
    {
        ssize_t bytes_read = body.size(); // The number of bytes we received in the body

        // Write the received body directly to the file
        ssize_t bytes_written = write(header.fd_ressource, &(body[0]), bytes_read);
        if (bytes_written == -1)
        {
            std::ostringstream oss;
            oss << "[method post] Error writing to file: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
            sendError(header, 500, fd); // Send internal server error if write fails
            return;
        }
        std::string head = "HTTP/1.1 204 No Content\r\n\r\n";
        {
            std::ostringstream oss;
            oss << "[method post] Transferred " << bytes_read << " bytes to " << header.ressourcePath;
            webservLogger.log(LVL_DEBUG, oss);
        }
        if (cgiList.find(fd) != cgiList.end())
            method_post_cgi(fd, header);
        else
        {
            if (-1 == send(fd, head.c_str(), head.size(), 0))
            {
                std::ostringstream oss;
                oss << "[method post] Error sending header";
                webservLogger.log(LVL_ERROR, oss);
            }
            // Check if we failed to read or write the full content
            if (bytes_written != bytes_read)
            {
                std::ostringstream oss;
                oss << "[method post] Error: Mismatch in bytes written.";
                webservLogger.log(LVL_ERROR, oss);
                sendError(header, 500, fd); // Send internal server error if there's a mismatch
                return;
            }
        }
    }
    else
    {
        // If the request is chunked, handle the chunked transfer encoding
        chunk[fd] = header;
        header.bodySize = 0;
        chunked_post(fd, body, header);
        return;
    }
    // Handle closing the connection if keep-alive is false
    if (!header.keepAlive)
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) 
        {
            std::ostringstream oss;
            oss << "Failed to remove fd from epoll: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
        }
        close(fd);
        fd_set.erase(fd); // Remove the file descriptor from the fd_set
    }
}