#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <map>

// Function to load a file into a string or vector of bytes
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


std::string get_request_path(const std::string &request) {
    std::istringstream request_stream(request);
    std::string method, path;
    request_stream >> method >> path;
    return path;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    const int PORT = 8080;
    
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
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }
    
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }
    
    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    while (true) 
    {
        // Accept an incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) 
        {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            close(server_fd);
            return 1;
        }

        // Buffer to store the incoming request
        char buffer[30000] = {0};
        read(new_socket, buffer, 30000);
        std::cout << "client request :\n\n" << buffer << std::endl << "\n******************\n";

        // Parse the request to determine the requested resource
        std::string request(buffer);
        std::string request_path = get_request_path(request);
        std::string mime_type = "text/plain"; // Default MIME type


        if (request_path == "/")// || request_path == "/index.html") 
        {
            request_path = "/index.html"; // Serve the index page by default
        } 
    
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
            send(new_socket, http_header.c_str(), http_header.size(), 0);

            // Send the file content
            send(new_socket, reinterpret_cast<const char*>(file_content.data()), file_content.size(), 0);
        } 
        else 
        {
            // Handle 404 Not Found
            std::string not_found_response = 
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: 58\r\n"
                "\r\n"
                "<html><body><h1>404 Not Found</h1><p>Resource not found.</p></body></html>";

            send(new_socket, not_found_response.c_str(), not_found_response.size(), 0);
        }

        // Close the connection
        close(new_socket);
    }
    close(server_fd);
    return 0;
}
