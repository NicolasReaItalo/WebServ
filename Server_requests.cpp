/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_method.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 12:00:24 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/02 12:00:25 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::method_get(std::string &request, int i)
{
    std::string request_path = get_request_path(request);
    std::string mime_type = "text/plain"; // Default MIME type
    if (request_path == "/")// || request_path == "/index.html") 
        request_path = "/index.html"; // Serve the index page by default

    // Serve the requested file
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

void Server::method_post(std::string &request, int i)
{
    std::string::size_type pos = request.find("\r\n\r\n");
        // if (pos != std::string::npos) 
        // {
            // std::string headers = request.substr(0, pos);
    std::string body = request.substr(pos + 4);
        // }
    std::string response_body = "POST data received : " + body;

    std::ostringstream oss;
    oss << response_body.size();

    std::string response_head = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " + oss.str() + "\r\n"
        "Connection: close\r\n\r\n" + "\r\n";
    
    send(fds[i].fd, response_head.c_str(), response_head.size(), 0);
    send(fds[i].fd, response_body.c_str(), response_body.size(), 0);
}

void Server::method_delete(std::string &request, int i)
{
    std::string request_path = get_request_path(request);
    if (request_path == "/")// || request_path == "/index.html") 
        request_path = "/index.html"; // Serve the index page by default

    // Delete the requested file
    std::remove(request_path.c_str());
    std::string http_header = 
        "HTTP/1.1 200 OK\r\n"
        "\r\n";

    // Send the HTTP header
    send(fds[i].fd, http_header.c_str(), http_header.size(), 0);
}
void Server::do_request(char* buffer, int i)
{
    std::cout << "client request :\n\n" << buffer << std::endl << "\n******************\n";
    std::string request(buffer);
    std::string request_type = get_method_type(request);
    if (request_type == "GET")
        method_get(request, i);
    if (request_type == "POST")
        method_post(request, i);
    if (request_type == "DELETE")
        method_delete(request, i);
}