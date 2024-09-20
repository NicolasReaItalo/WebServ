#include "Server.hpp"



int Server::ServerStart()
{
    list_size = servers.size();
    std::list<ConfigServer>::iterator it = servers.begin();

    //create the epoll
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "epoll_create1 failed: " << strerror(errno) << std::endl;
        close(epoll_fd);
        return 1;
    }
    fd_set[epoll_fd] = std::make_pair("0", "0");

    // For each Server initialize the servers sockets
    for (int i = 0; i < list_size; i++)
    {
        //creating the socket
        if ((server_fd[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
        std::cerr << "Socket failed: " << strerror(errno) << std::endl;
        return 1;
        }

        //configure the socket options
        int opt = 1;
        if (setsockopt(server_fd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT | SO_KEEPALIVE, &opt, sizeof(opt))) 
        {
            std::cerr << "setsockopt failed: " << strerror(errno) << std::endl;
            closeAllFd();
            return 1;
        }
        //setting the address
        address[i].sin_family = AF_INET;
        address[i].sin_addr.s_addr = inet_addr(it->getAddress().c_str());
        address[i].sin_port = htons(atoi(it->getPort().c_str()));

        //binding the server_fd on the correct address
        if (bind(server_fd[i], (struct sockaddr *)& address[i], sizeof(address[i])) < 0)
        {
            std::cerr << "Bind failed: " << strerror(errno) << std::endl;
            closeAllFd();
            return 1;
        }
        std::cout << "Bind successfull on " << server_fd[i] << std::endl;
        fd_set[server_fd[i]] = std::make_pair(it->getAddress(), it->getPort());
        //making server_fd a passive socket, to accept incoming connexion requests
        if (listen(server_fd[i], 32) < 0) 
        {
            std::cerr << "Listen failed: " << strerror(errno) << std::endl;
            closeAllFd();
            return 1;
        }
        // Add the server socket to the epoll instance
        epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = server_fd[i];
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd[i], &event) == -1) 
        {
            std::cerr << "epoll_ctl failed: " << strerror(errno) << std::endl;
            closeAllFd();
            return 1;
        }
        std::cout << "server online\nhost:port =" << it->getAddress() << ":" << it->getPort() << std::endl; 
		std::cout << "Server name =" << it->getServerNames().front() << std::endl;
        it++;
    }
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    int ret = 0;
    while (ret == 0 && stopper==0)
		ret = ServerRun();
	return (0);
}