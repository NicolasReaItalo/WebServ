#include "Server.hpp"
#include "webserv.hpp"

Server::Server()
{
	addrlen = sizeof(address);
    for (int i = 0; i < MAX_CLIENTS + 1; i++)
    {
        fds[i].fd = -1;
        fds[i].events = 0;
        fds[i].revents = 0;
    }
	PORT = 8080;
	nfds = 1;
    host = "127.0.0.1";

}
Server::Server(int port, std::string host) : PORT(port), host(host)
{
	addrlen = sizeof(address);
	nfds = 2;
}



int Server::run_server()
{
    int timeout_ms = 100;
    if (poll(fds, nfds, timeout_ms) == -1)
    {
        std::cerr << "poll() failed: " << strerror(errno) << std::endl;
        return 1;
    }
    if (stopper != 0)
            return 1;
    for (int i = 0; i < nfds; i++)
    {
        if (fds[i].revents & POLLIN)
        {
            if (fds[i].fd == server_fd)  // New incoming connection
            {
                int new_socket;
                if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
                    std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                    // continue;
                }

                // Add new socket to fds array
                if (nfds < MAX_CLIENTS + 1) 
                {
                    fds[nfds].fd = new_socket;
                    fds[nfds].events = POLLIN;  // Monitor this socket for incoming data
                    nfds++;
                    std::cout << "New connection, socket fd: " << new_socket << std::endl;
                } 
                else 
                {
                    std::cerr << "Max clients reached. Connection refused." << std::endl;
                    close(new_socket);
                }
            }
            else // Data from an existing connection
            {
                char buffer[BUFFER_SIZE] = {0};
                int valread = read(fds[i].fd, buffer, BUFFER_SIZE);
                if (valread == 0)
                {
                    std::cout << "Client disconnected, socket fd: " << fds[i].fd << std::endl;
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
                else if (valread > 0)
                {
                    do_request(buffer, i);
                }
            }
        }
    }
    // Clean up closed connections
    fds_cleanup();
    return 0;
}

void Server::fds_cleanup()
{
    for (int i = 1; i < nfds; i++) 
    {
        if (fds[i].fd == -1) 
        {
            // Shift all fds down
            for (int j = i; j < nfds - 1; j++) 
            {
                fds[j] = fds[j + 1];
            }
            nfds--;
            i--;  // Stay at the current index after the shift
        }
    }
}

int Server::server_cleanup()
{
    std::cout << "Shutting server down..." << std::endl;

	for (int i = 0; i < nfds; i++) 
    {
        if (fds[i].fd != -1) 
        {
            close(fds[i].fd);
        }
    }
    close(server_fd);
    std::cout << "Server offline, goodbye!" << std::endl;
    return 0;
}
int Server::start_server()
{
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        std::cerr << "Socket failed: " << strerror(errno) << std::endl;
        return 1;
    }
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        std::cerr << "setsockopt failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }
    fds[0].fd = STDIN_FILENO;	
	fds[0].events = POLLIN;
    fds[1].fd = server_fd;
    fds[1].events = POLLIN;
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host.c_str());
    address.sin_port = htons(PORT);
    
    if (bind(fds[1].fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(fds[1].fd);
        return 1;
    }
    
    if (listen(fds[1].fd, 3) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(fds[1].fd);
        return 1;
    }
    
    std::cout << "Server listening on port " << PORT << "..." << std::endl;
    int ret = 0;
	while (ret == 0)
    {
		ret = run_server();
    }
	return stop_server();
}

int Server::stop_server()
{
    fds_cleanup();
    server_cleanup();
    return 0;
}