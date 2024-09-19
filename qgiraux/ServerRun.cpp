#include "Server.hpp"

int set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "fcntl(F_GETFL) failed: " << strerror(errno) << std::endl;
        return 0;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "fcntl(F_SETFL) failed: " << strerror(errno) << std::endl;
        return 0;
    }
    return 1;
}

int Server::ServerRun()
{
    nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 10);
    if (nfds == -1)
	{
		std::cerr << "epoll_wait failed: " << strerror(errno) << std::endl;
		closeAllFd();
		return 1;
	}
    for (int i = 0; i < nfds; ++i)
    {
        int fd = events[i].data.fd;

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
                    new_socket = accept(fd, (struct sockaddr *)&client_address, &addrlen);
                    if (new_socket == -1)
                    {
                        std::cerr << "accept failed: " << strerror(errno) << std::endl;
                        break;
                    }
                    // Set the new socket to non-blocking mode
                    if (set_nonblocking(new_socket) == -1)
                    {
                        std::cerr << "set_nonblocking failed: " << strerror(errno) << std::endl;
                        close(new_socket);
                        break;
                    }

                    // Prepare and add the new socket to epoll
                    struct epoll_event event;
                    event.events = EPOLLIN | EPOLLET;
                    event.data.fd = new_socket;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1) 
                    {
                        std::cerr << "epoll_ctl failed: " << strerror(errno) << std::endl;
                        close(new_socket);
                        continue;
                    }
                    std::cout << "Accepted new connection, fd: " << new_socket << std::endl;
                    // fd_set.insert(new_socket);
                    fd_set[new_socket] = fd_set[fd];
                    break;
                }
            }
        }
        /*request entrante sur une connexion etablie*/
        else if (events[i].events & EPOLLIN)
        {
            receive_data(fd, i);
        }

        /*envoi d'une response en chunks*/
        else if (events[i].events & EPOLLOUT)
        {
            send_chunk(fd, i);
        }

        /*sonething went wrong*/
        // Handle errors
        else if (events[i].events & (EPOLLERR | EPOLLHUP))
        {
            std::cerr << "epoll error on fd: " << fd << std::endl;
            close(fd);
            fd_set.erase(fd);
        }
        else
            std::cout << "???????????\n";
    }
    return 0;
}