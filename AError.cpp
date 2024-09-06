
#include "Server.hpp"
#include <ctime>
#include <sstream>
#include <vector>

void Server::send_error(int error_code, int client_fd)
{
    std::map<int, std::string> errorCodeList = {{400, "Bad request"},{404, "Error not found"}};
    std::time_t clock = std::time(NULL);
        std::string time_str = std::ctime(&clock);
        time_str.erase(time_str.find_last_not_of("\n") + 1);
    std::stringstream link;
    link << "./" << error_code << ".html";
    std::vector<unsigned char> file_content = load_file(link.str());
    std::ostringstream oss;
    oss << file_content.size();
    std::map<int, std::string>::iterator it = errorCodeList.find(error_code);
    int i = it->first;
    std::string not_found_response = 
            "HTTP/1.1" + it->first + ':' + it->second + "\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + oss.str() + "\r\n"
            "time: " + time_str + "\r\n"
            "\r\n";

        send(client_fd, not_found_response.c_str(), not_found_response.size(), 0);
        send(client_fd, reinterpret_cast<const char*>(file_content.data()), file_content.size(), 0);
}