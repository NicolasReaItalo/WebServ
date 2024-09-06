/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRun.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 14:29:21 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/05 12:16:16 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

void set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "fcntl(F_GETFL) failed: " << strerror(errno) << std::endl;
        return ;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "fcntl(F_SETFL) failed: " << strerror(errno) << std::endl;
        return ;
    }
}

int Server::ServerRun()
{
	if (stopper != 0)
            return 1;
	nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 100);
	if (nfds == -1)
	{
		std::cerr << "epoll_wait failed: " << strerror(errno) << std::endl;
		close (server_fd);
		return 1;
	}
	for (int i = 0; i < nfds; ++i)
	{
		//if it is a new connexion
		if (events[i].data.fd == server_fd)
		{
			new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
			if (new_socket == -1)
			{
				std::cerr << "accept failed: " << strerror(errno) << std::endl;
                continue;
			}
			
			set_nonblocking(new_socket);
			ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = new_socket;
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &ev) == -1) 
			{
				std::cerr << "epoll_ctl failed: " << strerror(errno) << std::endl;
				close(new_socket);
				continue;
			}
			std::cout << "Accepted new connection, fd: " << new_socket << std::endl;
		}
		else 
		{
			char buffer[BUFFER_SIZE] = {0};
			// Handle I/O on an existing connection
			int client_fd = events[i].data.fd;
			ssize_t count = read(client_fd, buffer, BUFFER_SIZE);
			if (count == -1) {
				if (errno != EAGAIN) {
					std::cerr << "read failed: " << strerror(errno) << std::endl;
					close(client_fd);
				}
			} 
			else if (count == 0) 
			{
				// End of file, client disconnected
				std::cout << "Client disconnected, fd: " << client_fd << std::endl;
				close(client_fd);
			} 
			else 
			{
				// Process incoming data (echo back in this case)
				std::string request = buffer;
				// std::cout << "Received data: " << request << std::endl;
				method_get(client_fd, buffer);
			}
		}
	}
	return 0;
}