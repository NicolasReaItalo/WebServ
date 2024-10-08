#include "Server.hpp"


void Server::method_error(const header_infos& header, int fd)
{
    if (header.ressourcePath != "")
    {
        method_get(header, fd);
        return;
    }
    sendError(header, header.returnCode, fd);
    return ;
}
