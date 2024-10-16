#include "Server.hpp"


void Server::method_error(const header_infos& header, int fd)
{
    // if (header.ressourcePath != "")
    // {
    //     method_get(header, fd);
    //     return;
    // }
    sendError(header, header.returnCode, fd);
	if (cgiList.find(fd) != cgiList.end())
		cgiList.erase(fd);
	if (chunk.find(fd) != chunk.end())
		chunk.erase(fd);
	fd_set.erase(fd);
	close(fd);
    return ;
}
