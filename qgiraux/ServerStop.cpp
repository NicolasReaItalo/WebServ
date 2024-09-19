#include "Server.hpp"

void Server::closeAllFd()
{
    std::map<int, std::pair<std::string, std::string> >::iterator it;

    for (it = fd_set.begin(); it != fd_set.end(); it++)
        close(it->first);
}