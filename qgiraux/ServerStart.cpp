#include "Server.hpp"
#include "Logger.hpp"



int Server::ServerStart()
{
    time = std::time(NULL);
    list_size = servers.size();
    std::list<ConfigServer>::iterator it = servers.begin();
    //create the epoll
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
    {
        std::ostringstream oss;
        oss << "[serverStart] epoll_create1 failed: " << strerror(errno);
        webservLogger.log(LVL_ERROR, oss);
        close(epoll_fd);
        return 1;
    }
    
    fdsets tmp = {"0", "0", time, true, false};
    fd_set[epoll_fd] = tmp;

    // For each Server initialize the servers sockets
    for (int i = 0; i < list_size; i++)
    {
        //creating the socket
        if ((server_fd[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            std::ostringstream oss;
            oss << "[serverStart] Socket failed: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
            return 1;
        }

        //configure the socket options
        int opt = 1;
        if (setsockopt(server_fd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT | SO_KEEPALIVE, &opt, sizeof(opt))) 
        {
            std::ostringstream oss;
            oss << "[serverStart] setsockopt failed: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
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
            std::ostringstream oss;
            oss << "[serverStart] Bind failed: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
            closeAllFd();
            return 1;
        }
        {
            std::ostringstream oss;
            oss << "[serverStart] Bind successfull on " << server_fd[i];
            webservLogger.log(LVL_INFO, oss);
        }
        fdsets tmp = {it->getAddress(), it->getPort(), time, true, false};
        fd_set[server_fd[i]] = tmp;
        //making server_fd a passive socket, to accept incoming connexion requests
        if (listen(server_fd[i], 32) < 0) 
        {
            {
                std::ostringstream oss;
                oss << "[serverStart] Listen failed: " << strerror(errno);
                webservLogger.log(LVL_ERROR, oss);
            }
            closeAllFd();
            return 1;
        }
        // Add the server socket to the epoll instance
        epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = server_fd[i];
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd[i], &event) == -1) 
        {
            std::ostringstream oss;
            oss << "[serverStart] epoll_ctl failed: " << strerror(errno);
            webservLogger.log(LVL_ERROR, oss);
            closeAllFd();
            return 1;
        }
        {
            std::ostringstream oss;
            oss << "[serverStart] server online\n\t\thost:port = " << it->getAddress() << ":" << it->getPort();
            oss << "\n\t\tServer name =" << it->getServerNames().front();
            webservLogger.log(LVL_INFO, oss);
        }
        it++;
    }
    std::cout << "\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    int ret = 0;
    while (ret == 0 && stopper==0)
    {
		ret = ServerRun();
        time = std::time(NULL);

        std::map<int, fdsets>::iterator it;
        for (it = fd_set.begin(); it != fd_set.end();)
        {
            if (!it->second.listener && time - it->second.timer > 5)
            {
                int tmp = it->first;
                ++it;
                if (fd_set[tmp].client)
                {
                    std::ostringstream oss;
                    oss << "[serverRun] connexion timeout on fd " << tmp ;
                    webservLogger.log(LVL_INFO, oss);
                }
                close(tmp);
                fd_set.erase(tmp);
            }
            else
                ++it;
        }
    }
	return (0);
}