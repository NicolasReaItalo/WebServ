/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chunk_send.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:25 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/07 12:21:38 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "Server.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sstream>
#include <ctime>
#include <fstream>

void Server::send_chunk(int fd, int i, const header_infos& header)
{
    {
        std::ostringstream oss;
        oss << "[send chunk] sending body by chunks to " << fd << "...";
        webservLogger.log(LVL_DEBUG, oss);
    }
    /*if not in chunklist ==> send header, set EPOLLOUT and add fd to chunk*/
    if (chunk.find(fd) == chunk.end())
    {
        chunk[fd] = header;


        fd_set[chunk[fd].fd_ressource] = fd_set[fd];
        fd_set[chunk[fd].fd_ressource].listener = false;
        std::string mime = mimeList[get_mime_type(chunk[fd].ressourcePath)];

        /*create and send the header*/
        std::string head = "HTTP/1.1 200 OK\r\nContent-Type: " + mime + "\r\nTransfer-Encoding: chunked\r\n\r\n";
        {
            std::ostringstream oss;
            oss << "[send chunk] sending header to: " << fd;
            webservLogger.log(LVL_DEBUG, oss);
        }
        send(fd, head.c_str(), head.size(), 0);
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
    //if already in chunk list ==>send next chunk
    std::ifstream file(chunk[fd].ressourcePath.c_str(), std::ios::binary);
    if (!file) 
    {
        std::ostringstream oss;
        oss << "[send chunk] Error opening file: " << chunk[fd].ressourcePath;
        perror("error code :");
        webservLogger.log(LVL_ERROR, oss);
        return ; // Return an empty vector
    }

    file.seekg(chunk[fd].readIndex);
    std::vector<unsigned char> tmp(CHUNK_SIZE);
    std::streamsize bytesRead = file.read(reinterpret_cast<char*>(tmp.data()), CHUNK_SIZE).gcount();
    std::string data;
    std::stringstream oss;
    if (file.eof() || bytesRead < 0)
    {
        oss << std::hex << bytesRead << "\r\n"; 
        oss.write(reinterpret_cast<const char*>(&tmp[0]), bytesRead);
        oss << "\r\n";
        data = oss.str();
        {
            std::ostringstream oss;
            oss << "[send chunk] sending next chunk to " << fd;
            webservLogger.log(LVL_DEBUG, oss);
        }                
        ssize_t bytesSent = send(fd, data.c_str(), data.size(), 0);
        if (bytesSent == -1){

            return failed_to_send(fd);
        }
        {
            std::ostringstream oss;
            oss << "[send chunk] sending last chunk to " << fd;
            webservLogger.log(LVL_DEBUG, oss);
        }
        bytesSent = send(fd, "0\r\n\r\n", 5, 0);
        if (bytesSent == -1){

            return failed_to_send(fd);
        }
        events[i].events = EPOLLIN | EPOLLET;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &events[i]) == -1)
        {
            std::ostringstream oss;
            oss << "[send chunk] epoll_ctl failed: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
            close(fd);
            fd_set.erase(fd);
        }
        close(chunk[fd].fd_ressource);
        fd_set.erase(chunk[fd].fd_ressource);
        chunk.erase(fd);
        if (cgiList.find(fd) != cgiList.end())
        {
            remove (cgiList[fd].uri.c_str());
            cgiList.erase(fd);
        }
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
            std::ostringstream oss;
            oss << "[send chunk] epoll_ctl failed: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
            close(fd);
            fd_set.erase(fd);
            chunk.erase(fd);
            return;
        }
        chunk[fd].readIndex += CHUNK_SIZE;
    }
}
