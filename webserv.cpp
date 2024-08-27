/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 11:26:45 by qgiraux           #+#    #+#             */
/*   Updated: 2024/08/27 13:37:14 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Server.hpp"

int main() 
{
    Server server;
    int ret;

    ret = server.start_server();
    return ret;
}
  /*  int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    const int PORT = 8080;
    struct pollfd fds[MAX_CLIENTS];
    int nfds = 1;

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        std::cerr << "Socket failed: " << strerror(errno) << std::endl;
        return 1;
    }
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        std::cerr << "setsockopt failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(fds[0].fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(fds[0].fd);
        return 1;
    }
    
    if (listen(fds[0].fd, 3) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(fds[0].fd);
        return 1;
    }
    
    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    while (true) 
    {
        int poll_count = poll(fds, nfds, -1);

        if (poll_count == -1)
        {
            std::cerr << "poll() failed: " << strerror(errno) << std::endl;
            break;
        }
        
        for (int i = 0; i < nfds; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == server_fd)  // New incoming connection
                {
                    int new_socket;
                    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
                        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                        continue;
                    }

                    // Add new socket to fds array
                    if (nfds < MAX_CLIENTS) {
                        fds[nfds].fd = new_socket;
                        fds[nfds].events = POLLIN;  // Monitor this socket for incoming data
                        nfds++;
                        std::cout << "New connection, socket fd: " << new_socket << std::endl;
                    } else {
                        std::cerr << "Max clients reached. Connection refused." << std::endl;
                        close(new_socket);
                    }
                }
                else // Data from an existing connection
                {
                    char buffer[BUFFER_SIZE] = {0};
                    int valread = read(fds[i].fd, buffer, BUFFER_SIZE);
                    if (valread == 0)
                    {
                        std::cout << "Client disconnected, socket fd: " << fds[i].fd << std::endl;
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    }
                    else if (valread > 0)
                    {
                        std::cout << "client request :\n\n" << buffer << std::endl << "\n******************\n";
                        std::string request(buffer);
                        std::string request_path = get_request_path(request);
                        std::string mime_type = "text/plain"; // Default MIME type
                        if (request_path == "/")// || request_path == "/index.html") 
                        {
                            request_path = "/index.html"; // Serve the index page by default
                        } 
                        // Serve the requested file
                        std::cout << "request path is "<< request_path << std::endl;
                        std::vector<unsigned char> file_content = load_file("." + request_path);
                        if (!file_content.empty())
                        {
                            mime_type = get_mime_type(request_path);
                            std::ostringstream oss;
                            oss << file_content.size();
                            std::string http_header = 
                                "HTTP/1.1 200 OK\r\n"
                                "Content-Type: " + mime_type + "\r\n"
                                "Content-Length: " + oss.str() + "\r\n"
                                "\r\n";

                            // Send the HTTP header
                            send(fds[i].fd, http_header.c_str(), http_header.size(), 0);
                            // Send the file content
                            send(fds[i].fd, reinterpret_cast<const char*>(file_content.data()), file_content.size(), 0);
                        }
                        else 
                        {
                            std::string not_found_response = 
                                "HTTP/1.1 404 Not Found\r\n"
                                "Content-Type: text/html\r\n"
                                "Content-Length: 58\r\n"
                                "\r\n"
                                "<html><body><h1>404 Not Found</h1><p>Resource not found.</p></body></html>";

                            send(fds[i].fd, not_found_response.c_str(), not_found_response.size(), 0);
                        }
                    }
                }
            }
        }
        // Clean up closed connections
        for (int i = 0; i < nfds; i++) 
        {
            if (fds[i].fd == -1) 
            {
                // Shift all fds down
                for (int j = i; j < nfds - 1; j++) 
                {
                    fds[j] = fds[j + 1];
                }
                nfds--;
                i--;  // Stay at the current index after the shift
            }
        }
    }
    for (int i = 0; i < nfds; i++) 
    {
        if (fds[i].fd != -1) 
        {
            close(fds[i].fd);
        }
    }
    close(server_fd);
    return 0;
}*/
