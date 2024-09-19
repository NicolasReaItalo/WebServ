/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollin.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:44 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/19 18:11:02 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <vector>

/* Reads the data received from the client and either:
    - sends to the correct method or
    - receives and concatenates new chunk
> int fd  : the fd on which the data is received
> int i   : the index of the server to which the data is sent */
void Server::receive_data(int fd, int i)
{
    unsigned char buffer[maxBodySize]; // Buffer to read data (binary-safe)
    ssize_t bytesRead;
    std::vector<unsigned char> body;
    std::string headerStr = ""; // String to hold the header part

    bool headerParsed = false; // Flag to track whether the header has been parsed

    while (true) {
        bytesRead = read(fd, buffer, maxBodySize);

        if (bytesRead < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more data to read
                break;
            } else {
                // Read error
                std::cerr << "Read error on fd: " << fd << ", error: " << strerror(errno) << std::endl;
                close(fd);
                fd_set.erase(fd);
                return;
            }
        }
        if (bytesRead == 0) {
            // End of file, client disconnected
            std::cout << "Client disconnected, fd: " << fd << std::endl;
            close(fd);
            fd_set.erase(fd);
            return;
        }

        // If the header is not yet parsed, look for the header/body separation
        if (!headerParsed) {
            std::string tmp(reinterpret_cast<char*>(buffer), bytesRead);

            // Find the position where the header ends (marked by \r\n\r\n)
            size_t headerEndPos = tmp.find("\r\n\r\n");
            if (headerEndPos != std::string::npos) {
                // Extract header
                headerStr = tmp.substr(0, headerEndPos);

                // Convert remaining part to the body (binary-safe)
                std::vector<unsigned char> bodyPart(buffer + headerEndPos + 4, buffer + bytesRead);
                body.insert(body.end(), bodyPart.begin(), bodyPart.end());

                headerParsed = true; // Mark header as parsed

                // Parse header
                header_infos header = headerParser(headerStr, fd_set[fd]);

                // Now call the appropriate method based on the header
                switch (header.toDo)
                {
                    case POST:
                        method_post(header, body, fd, i);
                        return;
                    case GET:
                        method_get(header, fd, i);
                        return;
                    case DELETE:
                        method_delete(header, fd);
                        return;
                    default:
                        std::cerr << "Unknown method" << std::endl;
                        return;
                }
            } else {
                // The header is incomplete; append to headerStr and continue reading
                headerStr += tmp;
                continue;
            }
        } else {
            // After the header has been parsed, continue receiving the body
            body.insert(body.end(), buffer, buffer + bytesRead);
        }
    }
}






// void Server::receive_data(int fd, int i)
// {
//     unsigned char buffer[maxBodySize + 1]; // +1 to ensure null-termination
//     ssize_t bytesRead;

//     while (true) {
//         bytesRead = read(fd, buffer, maxBodySize);
//         std::cout << buffer;
//         if (bytesRead < 0) {
//             if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                 // No more data to read
//                 break;
//             } else {
//                 // Read error
//                 std::cerr << "Read error on fd: " << fd << ", error: " << strerror(errno) << std::endl;
//                 close(fd);
//                 fd_set.erase(fd);
//                 return;
//             }
//         }
//         if (bytesRead == 0) {
//             // End of file, client disconnected
//             std::cout << "Client disconnected, fd: " << fd << std::endl;
//             close(fd);
//             fd_set.erase(fd);
//             return;
//         }

//         buffer[bytesRead] = '\0'; // Null-terminate the buffer
//         std::string tmp(reinterpret_cast<char*>(buffer));
//         /*if the request is chunked*/
//         // if (is_fd_in_chunklist(fd) == true)
//         // {
//         //     chunked_post(fd, tmp);
//         //     continue;
//         // }

//         std::string body = "";
//         std::string head = tmp.substr(0, tmp.find("\r\n\r\n"));
//         if (tmp.size() > head.size())
//             body = tmp.substr(tmp.find("\r\n\r\n") + 4);
//         header_infos header = headerParser(head, fd_set[fd]);    
//         std::cout << body;
//         switch (header.toDo)
//         {
//             case POST:
//                 method_post(header, body, fd, i);
//                 return;
//             case GET :
//                 method_get(header, fd, i);
//                 return;
//             case DELETE :
//                 method_delete(header, fd);
//                 return;
//             default:
//                 std::cerr << "Unknown method" << std::endl;
//                 return;
//         }
//     }
// }