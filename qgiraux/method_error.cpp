#include "Server.hpp"


void Server::method_error(const header_infos& header, int fd, int i)
{
    if (header.ressourcePath != "")
    {
        method_get(header, fd, i);
        return;
    }
    sendError(header.returnCode, fd);
    return ;
}
