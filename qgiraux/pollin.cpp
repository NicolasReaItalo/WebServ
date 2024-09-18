#include "Server.hpp"

/*Reads the data received from the client and either :
    - sends to the correct method or
    - receive and concatenate new chunk

> int fd  : the fd on which the data is received
> int i   : the index of the server to which the data is sent*/
void Server::receive_data(int fd, int i)
{
    char buffer[maxBodySize + 1]; // +1 to ensure null-termination
    ssize_t bytesRead = read(fd, buffer, maxBodySize);
    if (bytesRead < 0) {
        return;
    }
    buffer[bytesRead] = '\0'; // Null-terminate the buffer
    std::string tmp(buffer);

    /*if the request is chunked*/
    if (is_fd_in_chunklist(fd) == true)
    {
        return chunked_post(fd, tmp);
    }
    std::string head = tmp.substr(0, tmp.find("\r\n\r\n"));
    std::string body = tmp.substr(tmp.find("\r\n") + 2);
    header_infos header = headerParser(head, fd_set[fd]);

    switch (header.toDo)
    {
        case POST:
            method_post(header, body, fd, i);
            break;
        case GET :
            method_get(header, fd, i);
            break;
        case DELETE :
            method_delete(header, fd);
            break;
        case GET_CGI :

            break;
        case POST_CGI :


        default:
            break;
    }
}
