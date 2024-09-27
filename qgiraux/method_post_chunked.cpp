/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_post_chunked.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:54:05 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/26 16:03:30 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::method_post_chunked(header_infos header, std::vector<unsigned char> body, int fd, int i)
{
    // Open the target file to write the body data
    header.fd_ressource = open(header.ressourcePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (-1 == header.fd_ressource)
    {
        std::cout << "Failed to open " << header.ressourcePath << std::endl;
        sendError(500, fd); // Send internal server error if file open fails
        return;
    }

    std::cout << "Managed to open " << header.ressourcePath << std::endl;
    fdsets tmp = {"0","0",time, false};
    fd_set[header.fd_ressource] = tmp;

    // If the request is not chunked (i.e., receiving the entire body in one go)
    if (!header.chunked)
    {
        ssize_t bytes_read = body.size(); // The number of bytes we received in the body

        // Write the received body directly to the file
        ssize_t bytes_written = write(header.fd_ressource, &(body[0]), bytes_read);
        if (bytes_written == -1)
        {
            std::cerr << "Error writing to file: " << strerror(errno) << std::endl;
            sendError(500, fd); // Send internal server error if write fails
            return;
        }
        std::string head = "HTTP/1.1 204 No Content\r\n\r\n";
        std::cout << "Transferred " << bytes_read << " bytes to " << header.ressourcePath << std::endl;
        if (-1 == send(fd, head.c_str(), head.size(), 0))
                std::cout << "error sending header\n";
        // Check if we failed to read or write the full content
        if (bytes_written != bytes_read)
        {
            std::cerr << "Error: Mismatch in bytes written." << std::endl;
            sendError(500, fd); // Send internal server error if there's a mismatch
            return;
        }
    }
    else
    {
        // If the request is chunked, handle the chunked transfer encoding
        chunk[fd] = header;
        receive_data(fd, i);
    }

    // Handle closing the connection if keep-alive is false
    if (!header.keepAlive)
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) 
        {
            std::cerr << "Failed to remove fd from epoll: " << strerror(errno) << std::endl;
        }
        close(fd);
        fd_set.erase(fd); // Remove the file descriptor from the fd_set
    }
}