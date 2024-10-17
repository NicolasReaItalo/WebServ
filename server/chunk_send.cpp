/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chunk_send.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:25 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/17 11:23:02 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "Server.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sstream>
#include <ctime>
#include <fstream>

bool Server::is_socket_open(int fd) {
    // Try to use fcntl to get the file descriptor flags
    if (fcntl(fd, F_GETFL) != -1) {
        return true;  // File descriptor is valid
    } else if (errno == EBADF) {
        return false; // File descriptor is closed
    }
    return false; // Some other error, treat as closed
}

void Server::send_chunk(int fd, const header_infos& header)
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

        if (fd_set.find(fd) == fd_set.end())
        {
            fdsets tmp = {"0", "0", time, true, false};
            fd_set[fd] = tmp;
        }
        std::string mime = mimeList[get_mime_type(chunk[fd].ressourcePath)];

        /*create and send the header*/
        std::string head = "HTTP/1.1 200 OK\r\nContent-Type: " + mime + "\r\nTransfer-Encoding: chunked\r\n\r\n";
        {
            std::ostringstream oss;
            oss << "[send chunk] sending header to: " << fd;
            webservLogger.log(LVL_DEBUG, oss);
        }
        ssize_t bytesSent = send(fd, head.c_str(), head.size(), MSG_NOSIGNAL | MSG_DONTWAIT);
        if (bytesSent == -1)
        {
            std::ostringstream oss;
            oss << "[send chunk] Error sending header: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
            close(fd);
            fd_set.erase(fd);
            return;
        }
        events[chunk[fd].i_ev].events = EPOLLOUT | EPOLLET;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &events[chunk[fd].i_ev]) == -1)
        {
            std::ostringstream oss;
            oss << "epoll_ctl failed: " << strerror(errno);
            webservLogger.log(LVL_DEBUG, oss);
            close(fd);
            fd_set.erase(fd);
        }
        chunk[fd].readIndex = 0;
    }
    else
        send_chunk(fd);
}
void Server::send_chunk(int fd)
{
    //if already in chunk list ==>send next chunk
    std::ifstream file(chunk[fd].ressourcePath.c_str(), std::ios::binary);
    if (!file) 
    {
        std::ostringstream oss;
        oss << "[send chunk] Error opening file: \"" << chunk[fd].ressourcePath << "\" for fd " << fd << std::endl;
        perror("error code :");
        sendError(chunk[fd], 500, fd);
        webservLogger.log(LVL_ERROR, oss);
        
        return ; // Return an empty vector
    }

    file.seekg(chunk[fd].readIndex);
    std::vector<unsigned char> tmp(CHUNK_SIZE);
    std::streamsize bytesRead = file.read(reinterpret_cast<char*>(tmp.data()), CHUNK_SIZE).gcount();
    std::string data;
    std::stringstream oss;
    if (file.eof()/* || bytesRead < CHUNK_SIZE*/)
    {
        oss << std::hex << bytesRead << "\r\n"; 
        oss.write(reinterpret_cast<const char*>(&tmp[0]), bytesRead);
        oss << "\r\n";
        data = oss.str();                
        ssize_t bytesSent = send(fd, data.c_str(), data.size(), MSG_NOSIGNAL | MSG_DONTWAIT);
        if (bytesSent == -1){

            return failed_to_send(fd);
        }
        {
            std::ostringstream oss;
            oss << "[send chunk] sending last chunk to " << fd;
            webservLogger.log(LVL_DEBUG, oss);
        }
        bytesSent = send(fd, "0\r\n\r\n", 5, MSG_NOSIGNAL | MSG_DONTWAIT);
        if (bytesSent == -1){

            return failed_to_send(fd);
        }
        events[chunk[fd].i_ev].events = EPOLLIN | EPOLLET;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &events[chunk[fd].i_ev]) == -1)
        {
            std::ostringstream oss;
            oss << "[send chunk] epoll_ctl failed: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
            close(fd);
            fd_set.erase(fd);
            return;
        }
        chunk.erase(fd);
        if (cgiList.find(fd) != cgiList.end())
        {
            remove(cgiList[fd].uri.c_str());
            cgiList.erase(fd);
        }
    }
                    
    else
    {
        oss << std::hex << bytesRead << "\r\n"; 
        oss.write(reinterpret_cast<const char*>(tmp.data()), bytesRead);
        oss << "\r\n";
        data = oss.str();
        if (!is_socket_open(fd))
        {
            std::ostringstream oss;
            oss << "[send chunk] fd " << fd << " unavailable: ";
            webservLogger.log(LVL_ERROR, oss);
            close(fd);
            fd_set.erase(fd);
            chunk.erase(fd);
            return;
        }
        ssize_t bytesSent = send(fd, data.c_str(), data.size(), MSG_NOSIGNAL | MSG_DONTWAIT);
        if (bytesSent == -1)
        {
            return failed_to_send(fd);
        }
        events[chunk[fd].i_ev].events = EPOLLOUT | EPOLLET;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &events[chunk[fd].i_ev]) == -1)
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
