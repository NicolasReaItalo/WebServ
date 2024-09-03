/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_requests.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 12:00:24 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/03 10:25:02 by qgiraux          ###   ########.fr       */
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
        if (pos != std::string::npos) 
        {
            std::string headers = request.substr(0, pos);
            std::string body = request.substr(pos + 4);
            std::cout << "POST data received : " + body;
            std::string response = "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 48\r\n"
            "\r\n"
            "<html><body><h1>POST Received</h1></body></html>";
            
            send(fds[i].fd, response.c_str(), response.size(), 0);
        }
        else
        {
        // If no POST data is found, send an error response
        std::string bad_request_response = 
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 49\r\n"
            "\r\n"
            "<html><body><h1>400 Bad Request</h1></body></html>";

        send(fds[i].fd, bad_request_response.c_str(), bad_request_response.size(), 0);
    }
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
    // std::cout << "client request :\n\n" << buffer << std::endl << "\n******************\n";
    std::string request(buffer);
    std::string request_type = get_method_type(request);
    if (request_type == "GET")
        method_get(request, i);
    if (request_type == "POST")
        method_post(request, i);
    if (request_type == "DELETE")
        method_delete(request, i);
}