/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chunk_send.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:25 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/20 13:29:37 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "Server.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sstream>
#include <ctime>
#include <fstream>

void Server::send_chunk(int fd, int i, header_infos header)
{
    std::cout << "*****GET chunker!!************\n";
    /*if not in chunklist ==> send header, set EPOLLOUT and add fd to chunk*/
    if (chunk.find(fd) == chunk.end())
    {
        chunk[fd] = header;

        // chunk[fd].fd_ressource = open(chunk[fd].ressourcePath.c_str(), O_RDONLY);

        //fd_set.insert(chunk[fd].fd_ressource);
        fd_set[chunk[fd].fd_ressource] = fd_set[fd];
        std::string mime = mimeList[get_mime_type(chunk[fd].ressourcePath)];

        /*create and send the header*/
        std::string head = "HTTP/1.1 200 OK\r\nContent-Type: " + mime + "\r\nTransfer-Encoding: chunked\r\n\r\n";
        send(fd, head.c_str(), head.size(), 0);
        // std::cout << head.c_str();
        events[i].events = EPOLLOUT | EPOLLET;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &events[i]) == -1)
        {
            std::cerr << "epoll_ctl failed: " << strerror(errno) << std::endl;
            close(fd);
            fd_set.erase(fd);
        }
        chunk[fd].readIndex = 0;
    }
}
void Server::send_chunk(int fd, int i)
{
    // std::cout << "chunk from data " << chunk[fd].readIndex << std::endl; 
    //if already in chunk list ==>send next chunk
    std::ifstream file(chunk[fd].ressourcePath.c_str(), std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << chunk[fd].ressourcePath << std::endl;
        return ; // Return an empty vector
    }

    file.seekg(chunk[fd].readIndex);
    std::vector<unsigned char> tmp(CHUNK_SIZE);
    std::streamsize bytesRead = file.read(reinterpret_cast<char*>(tmp.data()), CHUNK_SIZE).gcount();
    std::string data;
    std::stringstream oss;
    if (file.eof() || bytesRead < (int)maxBodySize)
    {
        oss << std::hex << bytesRead << "\r\n"; 
        oss.write(reinterpret_cast<const char*>(&tmp[0]), bytesRead);
        oss << "\r\n";
        data = oss.str();
                        
        ssize_t bytesSent = send(fd, data.c_str(), data.size(), 0);
        if (bytesSent == -1){

            return failed_to_send(fd);
        }
        bytesSent = send(fd, "0\r\n\r\n", 5, 0);
        if (bytesSent == -1){

            return failed_to_send(fd);
        }
        std::cout << "DFONE!!" << std::endl;
        events[i].events = EPOLLIN | EPOLLET;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &events[i]) == -1)
        {
            std::cerr << "epoll_ctl failed: " << strerror(errno) << std::endl;
            close(fd);
            fd_set.erase(fd);
        }
        close(chunk[fd].fd_ressource);
        fd_set.erase(chunk[fd].fd_ressource);
        chunk.erase(fd);
    }
                    
    else
    {
        oss << std::hex << bytesRead << "\r\n"; 
        oss.write(reinterpret_cast<const char*>(tmp.data()), bytesRead);
        oss << "\r\n";
        data = oss.str();
        ssize_t bytesSent = send(fd, data.c_str(), data.size(), MSG_MORE);
        if (bytesSent == -1){

            return failed_to_send(fd);
        }
        events[i].events = EPOLLOUT | EPOLLET;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &events[i]) == -1)
        {
            std::cerr << "epoll_ctl failed: " << strerror(errno) << std::endl;
            close(fd);
            fd_set.erase(fd);
            chunk.erase(fd);
            return;
        }
        chunk[fd].readIndex += CHUNK_SIZE;
    }
}
