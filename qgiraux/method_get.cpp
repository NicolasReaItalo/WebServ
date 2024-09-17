#include "Server.hpp"
#include <ctime>
#include <sstream>

void Server::method_get(header_infos header, int fd, int i)
{
    if (header.bodySize > maxBodySize)
    {
        send_chunk(fd, i, header);
    }
    else
    {
        header.fd_ressource = open(header.ressourcePath.c_str(), O_RDONLY, 0);
        fd_set.insert(header.fd_ressource);
        char buffer[maxBodySize];
        read(header.fd_ressource, buffer, header.bodySize);
        std::string body = buffer;
        std::time_t clock = std::time(NULL);
        std::string time_str = std::ctime(&clock);
        time_str.erase(time_str.find_last_not_of("\n") + 1);
        std::string mime = mimeList[get_mime_type(header.ressourcePath)];

        std::stringstream ss;
        ss  << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: " << mime << "\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "time: " << time_str << "\r\n" << "\r\n";

        std::string head = ss.str();
        send(fd, head.c_str(), head.size(), 0);
        send(fd, body.c_str(), body.size(), 0);
    }
}