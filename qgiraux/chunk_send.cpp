

#include "Server.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sstream>
#include <ctime>

void Server::send_chunk(int fd, int i, header_infos header)
{
    /*if not in chunklist ==> send header, set EPOLLOUT and add fd to chunk*/
    if (chunk.find(fd) == chunk.end())
    {
        chunk[fd] = header;
        chunk[fd].fd_ressource = open(chunk[fd].ressourcePath.c_str(), O_RDONLY);
        //fd_set.insert(chunk[fd].fd_ressource);
        fd_set[chunk[fd].fd_ressource] = fd_set[fd];
        //struct stat fileInfo;
        std::time_t clock = std::time(NULL);
        std::string time_str = std::ctime(&clock);
        time_str.erase(time_str.find_last_not_of("\n") + 1);
        std::string mime = mimeList[get_mime_type(chunk[fd].ressourcePath)];

        /*create and send the header*/
        std::string head = "HTTP/1.1 200 OK\r\nContent-Type: " + mime + 
        "\r\nTransfer-Encoding: chunked" + 
        "\r\ntime: " + time_str + "\r\n\r\n";
        send(fd, head.c_str(), head.size(), 0);
        events[i].events = EPOLLOUT | EPOLLET;
    }
}
void Server::send_chunk(int fd, int i)
{
    //if already in chunk list ==>send next chunk
    char tmp[CHUNK_SIZE];
    read(chunk[fd].fd_ressource, tmp, CHUNK_SIZE);
    std::string data(tmp);
    std::stringstream oss;
    if (data.size() < CHUNK_SIZE)
    {
        events[i].events = EPOLLIN | EPOLLET;
        oss << data.size() << "\r\n" << data << "\r\n";
		data = oss.str();
        send(fd, data.c_str(), data.size(), 0);
        send(fd, "0\r\n\r\n", 5, 0);
        close(chunk[fd].fd_ressource);
        fd_set.erase(chunk[fd].fd_ressource);
        chunk.erase(fd);
    }
    else
    {
        oss << data.size() << "\r\n" << data << "\r\n";
		data = oss.str();
        send(fd, data.c_str(), data.size(), 0);
    }
}