/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 11:27:16 by qgiraux           #+#    #+#             */
/*   Updated: 2024/08/27 12:15:31 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "webserv.hpp"

std::vector<unsigned char> load_file(const std::string &filename) 
{
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return std::vector<unsigned char>(); // Return an empty vector
    }
    
    std::vector<unsigned char> file_data((std::istreambuf_iterator<char>(file)),
                                           std::istreambuf_iterator<char>());
    return file_data;
}

// Function to get the MIME type based on the file extension
std::string get_mime_type(const std::string &path) 
{
    std::map<std::string, std::string> mime_types;
    mime_types[".html"] = "text/html";
    mime_types[".htm"] = "text/html";
    mime_types[".png"] = "image/png";
    mime_types[".jpg"] = "image/jpeg";
    mime_types[".jpeg"] = "image/jpeg";
    mime_types[".gif"] = "image/gif";
    mime_types[".css"] = "text/css";
    mime_types[".js"] = "application/javascript";
    
    std::string::size_type idx = path.rfind('.');
    if (idx != std::string::npos) {
        std::string extension = path.substr(idx);
        if (mime_types.find(extension) != mime_types.end()) {
            return mime_types[extension];
        }
    }
    
    return "application/octet-stream"; // Default for binary data
}

std::string get_request_path(const std::string &request) 
{
    std::istringstream request_stream(request);
    std::string method, path;
    request_stream >> method >> path;
    return path;
}