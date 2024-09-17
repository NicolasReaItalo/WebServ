#include "Server.hpp"


Server::Server(std::list<ServerConfig> servers) : servers(servers)
{
    list_size = servers.size();
    std::vector<int> vect(list_size);
    std::vector<struct sockaddr_in> vect2(list_size);
    server_fd = vect;
    address = vect2;
    nfds = 0;
    maxBodySize = BUFFER_SIZE;
    set_mimeList();
    set_errorList();

}