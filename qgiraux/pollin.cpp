#include "Server.hpp"

/*Reads the data received from the client and either : 
    - sends to the correct method or
    - receive and concatenate new chunk

> int fd  : the fd on which the data is received
> int i   : the index of the server to which the data is sent*/
void Server::receive_data(int fd, int i)
{
    char buffer[maxBodySize + 1]; // +1 to ensure null-termination
    ssize_t bytesRead;

    while (true) {
        bytesRead = read(fd, buffer, maxBodySize);
        if (bytesRead < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more data to read
                break;
            } else {
                // Read error
                std::cerr << "Read error on fd: " << fd << ", error: " << strerror(errno) << std::endl;
                close(fd);
                fd_set.erase(fd);
                return;
            }
        }
        if (bytesRead == 0) {
            // End of file, client disconnected
            std::cout << "Client disconnected, fd: " << fd << std::endl;
            close(fd);
            fd_set.erase(fd);
            return;
        }

        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        std::string tmp(buffer);

        /*if the request is chunked*/
        // if (is_fd_in_chunklist(fd) == true)
        // {
        //     chunked_post(fd, tmp);
        //     continue;
        // }

        std::string body = "";
        std::string head = tmp.substr(0, tmp.find("\r\n\r\n"));
        if (tmp.size() > head.size())
            body = tmp.substr(tmp.find("\r\n\r\n") + 4);

        header_infos header = headerParser(head, fd_set[fd]);    

        switch (header.toDo)
        {
            case POST:
                method_post(header, body, fd, i);
                return;
            case GET :
                method_get(header, fd, i);
                return;
            case DELETE :
                method_delete(header, fd);
                return;
            default:
                std::cerr << "Unknown method" << std::endl;
                return;
        }
    }
}