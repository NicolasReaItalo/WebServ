#include "Server.hpp"

void Server::method_post(header_infos header, std::string body, int fd, int i)
{
    header.fd_ressource = open(header.ressourcePath.c_str(), O_WRONLY);
    fd_set.insert(header.fd_ressource);
            if (header.chunked == false)
            {
                if (body.size() != header.bodySize)
                {
                                                        //return error 400 bad request
                    sendError(400, header.fd_ressource, header);
                    return ;
                }
                write(header.fd_ressource, body.c_str(), body.length());
                                                        //return 201 created
                sendError(201, header.fd_ressource, header);
            }
            else
            {
                chunk[fd] = header;
                receive_data(fd, i);
            }
}