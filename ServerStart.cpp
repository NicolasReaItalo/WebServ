#include "Server.hpp"

int Server::ServerStart()
{
	//creating a socket on server_fd
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        std::cerr << "Socket failed: " << strerror(errno) << std::endl;
        return 1;
    }
	//configuration of the socket server_fd
	int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT | SO_KEEPALIVE, &opt, sizeof(opt))) 
    {
        std::cerr << "setsockopt failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

	//setting the address
	address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host.c_str());
    address.sin_port = htons(port);
	//setting the server_fd on the correct address
	if (bind(server_fd, (struct sockaddr *)& address, sizeof(address)) < 0)
	{
		std::cerr << "Bind failed: " << strerror(errno) << std::endl;
		close (server_fd);
		return 1;
	}
	//making server_fd a passive socket, to accept incoming connexion requests
	if (listen(server_fd, 32) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

	//create the epoll
	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
        std::cerr << "epoll_create1 failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

	// Add the server socket to the epoll instance
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        std::cerr << "epoll_ctl failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }
    std::cout << "server online\nhost:port =" << host << ":" << port << std::endl; 
    int ret = 0;
	while (ret == 0)
		ret = ServerRun();
	close(server_fd);
	return 0;
}