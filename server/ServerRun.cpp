/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRun.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:52 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/14 12:43:12 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <ctime>

static int set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) 
    {
        std::ostringstream oss;
        oss << "[set_nonblocking] fcntl(F_GETFL) failed: " << strerror(errno);
        webservLogger.log(LVL_ERROR, oss);
        return 0;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) 
    {
        std::ostringstream oss;
        oss << "[set_nonblocking] fcntl(F_SETFL) failed: " << strerror(errno);
        webservLogger.log(LVL_ERROR, oss);
        return 0;
    }
    {
        std::ostringstream oss;
        oss << "[set_nonblocking] Socket " << sockfd << " set to non-blocking";
        webservLogger.log(LVL_DEBUG, oss);
    }
    return 1;
}

int Server::ServerRun()
{
    nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 100);
    if (stopper == 1)
        return 1;
    if (nfds == -1)
	{
        std::ostringstream oss;
        oss << "[serverRun] epoll_wait failed: " << strerror(errno);
        webservLogger.log(LVL_ERROR, oss);
		closeAllFd();
		return 1;
	}
    for (int i = 0; i < nfds; ++i)
    {
        int fd = events[i].data.fd;
        fd_set[fd].timer = time;

        // Check if the event is for a listening socket
        bool is_server_socket = std::find(server_fd.begin(), server_fd.end(), fd) != server_fd.end();

        if (is_server_socket)
        {
            // Handle new incoming connections
            for (size_t j = 0; j < server_fd.size(); ++j)
            {
                /*request for new connexion*/
                if (fd == server_fd[j])
                {
                    struct sockaddr_in client_address;
                    socklen_t addrlen = sizeof(client_address);
                    set_nonblocking(fd);
                    new_socket = accept(fd, (struct sockaddr *)&client_address, &addrlen);
                    if (new_socket == -1)
                    {
                        std::ostringstream oss;
                        oss << "[serverRun] accept failed: " << strerror(errno);
                        webservLogger.log(LVL_ERROR, oss);
                        break;
                    }
                    if (new_socket == 0)
                    {
                        std::ostringstream oss;
                        oss << "[serverRun] accept failed: accepted on 0";
                        webservLogger.log(LVL_ERROR, oss);
                        break;
                    }
                    // Set the new socket to non-blocking mode
                    if (set_nonblocking(new_socket) == -1)
                    {
                        std::ostringstream oss;
                        oss << "[serverRun] set_nonblocking failed: " << strerror(errno);
                        webservLogger.log(LVL_ERROR, oss);
                        close(new_socket);
                        break;
                    }

                    // Prepare and add the new socket to epoll
                    struct epoll_event event;
                    event.events = EPOLLIN | EPOLLET;
                    event.data.fd = new_socket;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1) 
                    {
                        std::ostringstream oss;
                        oss << "[serverRun] epoll_ctl failed: " << strerror(errno);
                        webservLogger.log(LVL_ERROR, oss);
                        close(new_socket);
                        continue;
                    }
                    
                    fd_set[new_socket] = fd_set[fd];
                    fd_set[new_socket].listener = false;
                    fd_set[new_socket].client = true;
                    {
                        std::ostringstream oss;
                        oss << "[serverRun] Accepted new connection, fd: " << new_socket;
                        webservLogger.log(LVL_INFO, oss);
                        continue;
                    }
                    break;
                }
            }
        }
        /*request entrante sur une connexion etablie*/
        else if (events[i].events & EPOLLIN)
        {
            std::cout << "got here\n";
            receive_data(fd, i);
        }

        /*envoi d'une response en chunks*/
        else if (events[i].events & EPOLLOUT)
        {
            send_chunk(fd);
        }

        /*sonething went wrong*/
        // Handle errors
        else if (events[i].events & (EPOLLERR | EPOLLHUP))
        {
            std::ostringstream oss;
            oss << "epoll error on fd: " << fd;
            webservLogger.log(LVL_ERROR, oss);
            close(fd);
            fd_set.erase(fd);
        }
        else
            std::cout << "???????????\nYOU SHOULD NEVER SEE THAT!!\n";
    }
    return 0;
}