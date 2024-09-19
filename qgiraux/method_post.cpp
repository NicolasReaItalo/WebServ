/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_post.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:40 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/19 18:34:08 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iomanip>


void Server::method_post(header_infos header, std::vector<unsigned char> body, int fd, int i)
{
    // Open the target file to write the body data
    header.fd_ressource = open(header.ressourcePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (-1 == header.fd_ressource)
    {
        std::cout << "Failed to open " << header.ressourcePath << std::endl;
        sendError(500, fd, header); // Send internal server error if file open fails
        return;
    }

    std::cout << "Managed to open " << header.ressourcePath << std::endl;
    fd_set[header.fd_ressource] = std::make_pair("0", "0");

    // If the request is not chunked (i.e., receiving the entire body in one go)
    if (!header.chunked)
    {
        // Check if the body exceeds the maximum allowed size
        if (body.size() >= maxBodySize)
        {
            sendError(413, fd, header); // Return error 413 (Payload Too Large)
            return;
        }
        ssize_t bytes_read = body.size(); // The number of bytes we received in the body

        // Write the received body directly to the file
        ssize_t bytes_written = write(header.fd_ressource, &(body[0]), bytes_read);
        if (bytes_written == -1)
        {
            std::cerr << "Error writing to file: " << strerror(errno) << std::endl;
            sendError(500, fd, header); // Send internal server error if write fails
            return;
        }

        std::cout << "Transferred " << bytes_read << " bytes to " << header.ressourcePath << std::endl;

        // Check if we failed to read or write the full content
        if (bytes_written != bytes_read)
        {
            std::cerr << "Error: Mismatch in bytes written." << std::endl;
            sendError(500, fd, header); // Send internal server error if there's a mismatch
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


// void Server::method_post(header_infos header, std::vector<unsigned char> body, int fd, int i)
// {
//     header.fd_ressource = open(header.ressourcePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
//     // fd_set.insert(header.fd_ressource);
//     if (-1 == header.fd_ressource)
//     {
//         std::cout << "failed to open " << header.ressourcePath << std::endl;
//         sendError(500, fd, header);
//         return;
//     }
//     std::cout << "managed to open  " << header.ressourcePath << std::endl;
//     fd_set[header.fd_ressource] = std::make_pair("0", "0");

    
//     if (!header.chunked)
//     {
//         if (body.size() >= maxBodySize)//(body.size() != header.bodySize)
//         {
//                                                 //return error 400 bad request
//             sendError(413, fd, header);
//             return ;
//         }
//         ssize_t bytes_read = body.size();                    // Number of bytes read from source_fd

//         // // Read from source_fd and write to target_fd in a loop
//         // while ((bytes_read = read(fd, &buffer[0], buffer.size())) > 0)
//         // {
        
//             ssize_t bytes_written = write(header.fd_ressource, &body[0], bytes_read);
//             if (bytes_written == -1)
//             {
//                 std::cerr << "Error writing to target_fd: " << strerror(errno) << std::endl;
//                 return;
//             }

//             std::cout << "Transferred " << bytes_read << " bytes from source_fd to target_fd." << std::endl;
//         // }
//         // Handle any errors from read()
//         if (bytes_read == -1)
//         {
//             std::cerr << "Error reading from source_fd: " << strerror(errno) << std::endl;
//             sendError(201, fd, header);
//         }
    
//     }
//     else
//     {
//         chunk[fd] = header;
//         receive_data(fd, i);
//     }
//     if (header.keepAlive == false)
//         {
//             if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) 
//             {
//                 std::cerr << "Failed to remove fd from epoll: " << strerror(errno) << std::endl;
//             }
//             close(fd);
//             fd_set.erase(fd);
//         }
// }