#include "Server.hpp"
#include <fcntl.h>
#include <ctime>
#include <sstream>

void Server::method_get(header_infos header, int fd, int i)
{
    if (header.bodySize > CHUNK_SIZE)
    {
        send_chunk(fd, i, header);
        return ;
    }
    else
    {
        std::cout << "in GET " << header.ressourcePath << "\n";
        std::vector<unsigned char> data = load_file(header.ressourcePath);
        // header.fd_ressource = open(header.ressourcePath.c_str(), O_RDONLY, 0);
        // fd_set[header.fd_ressource] = std::make_pair("0", "0");
        char buffer[maxBodySize];
        for (unsigned long i = 0; i < maxBodySize; i++)
            buffer[i] = 0;
        // read(header.fd_ressource, buffer, header.bodySize);
        // std::string body = buffer;
        std::time_t clock = std::time(NULL);
        std::string time_str = std::ctime(&clock);
        time_str.erase(time_str.find_last_not_of("\n") + 1);
        std::string mime = mimeList[get_mime_type(header.ressourcePath)];

        std::stringstream ss;
        ss  << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: " << mime << "\r\n"
        << "Content-Length: " << header.bodySize << "\r\n"
        << "time: " << time_str << "\r\n" << "\r\n";

        std::string head = ss.str();
        if (fcntl(fd, F_GETFD) != -1)
        {
            if (-1 == send(fd, head.c_str(), head.size(), 0))
                std::cout << "error sending header\n";
            
            if (-1 == send(fd, reinterpret_cast<const char*>(data.data()), header.bodySize, 0))
                std::cout << "error sending body\n";
        }
        if (header.keepAlive == false)
        {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) 
            {
                std::cerr << "Failed to remove fd from epoll: " << strerror(errno) << std::endl;
            }
            close(fd);
            fd_set.erase(fd);
        }
    }
}