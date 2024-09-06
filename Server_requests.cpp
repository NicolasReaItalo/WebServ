/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_requests.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 12:00:24 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/06 17:41:16 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <ctime>
#include <sstream>
#include <fstream>
#include <map>

std::vector<unsigned char> Server::load_file(const std::string &filename) 
{
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return std::vector<unsigned char>(); // Return an empty vector
    }
    std::cout << " opening file " << filename << std::endl;
    std::vector<unsigned char> file_data((std::istreambuf_iterator<char>(file)),
                                           std::istreambuf_iterator<char>());
    return file_data;
}

std::string get_request_path(const std::string &request) 
{
    std::istringstream request_stream(request);
    std::string method, path;
    request_stream >> method >> path;
    
    return path;
}

void Server::method_get(int client_fd, char* request)
{   
    std::map<std::string, std::string> header = headerRequestParser(request);
    if (get_mime_type(header["address"]).empty()){
        send_error(415, client_fd);
        return;
    }
    std::string path = get_request_path(request);
    if (path == "/")// root request_path == "/index.html") 
        path = d_path + d_main; // Serve the index page by default
    else
        path = d_path + path;

    // Serve the requested file
    std::vector<unsigned char> file_content = load_file("." + path);
    if (!file_content.empty())
    {
        send_header(client_fd, file_content.size(), path);
        // Send the file content
        if (-1 == send(client_fd, reinterpret_cast<const char*>(file_content.data()), file_content.size(), 0))
            send_error(400, client_fd);
    }
    // If no POST data is found, send an error response
    else  
        send_error(404, client_fd);
}