#include "Server.hpp"


void Server::method_error(header_infos header, int fd, int i)
{

    std::cout << header.ressourcePath << std::endl;
    if (header.ressourcePath == "")
    {
        method_get(header, fd, i);
        return;
    }
    sendError(header.returnCode, fd, header);
    return ;
}