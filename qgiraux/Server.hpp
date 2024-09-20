
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <list>
#include <vector>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ConfigServer.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <set>

#define MAX_EVENTS 500
#define BUFFER_SIZE 10000
#define CHUNK_SIZE 10000

#define ERROR 1
#define GET 2
#define POST 3
#define DELETE 6
#define GET_CGI 7
#define POST_CGI 8


typedef struct s_header_infos
{
	int toDo;
	unsigned long bodySize;
	std::string ressourcePath;
	std::string contentType;
	std::string interpreterPath;
	int returnCode;
	bool keepAlive;
	bool chunked;
	std::string queryParams;
	int timestamp;
	int fd_ressource;
	int cgi_pid;
	int locationIndex;
	class ConfigServer *configServer;
} header_infos;

class Server
{
    private :

        int list_size;
        std::list<ConfigServer> servers;
        std::vector<int> server_fd;
        int epoll_fd, new_socket;
        int nfds;
        std::set<int> fd_set;
        std::vector<struct sockaddr_in> address;
        std::vector<struct epoll_event> ev;
        struct epoll_event events[MAX_EVENTS];

        std::map<std::string, std::string> mimeList;
        std::map<int, std::string> errorList;
        std::map<int, header_infos> chunk;
        unsigned long maxBodySize;

        void closeAllFd();
        std::map<std::string, std::string> headerRequestParser(const std::string& request);
        void set_mimeList();
        void set_errorList();
        /*on an event pollin*/
        void receive_data(int fd, int i);

        void method_get(header_infos header, int fd, int i);
        void method_post(header_infos header, std::string body, int fd, int i);
        void method_delete(header_infos header, int fd);
        
        std::string get_mime_type(const std::string &uri);
        header_infos headerParser(std::string rawBuffer);

        void chunked_post(int fd, std::string tmp);
        void send_chunk(int fd, int i, header_infos header);
        void send_chunk(int fd, int i);

        bool is_fd_in_chunklist(int fd);
        void sendError(int errcode, int fd, header_infos header);
        std::string generate_error_page(int errcode);

    public :
        int ServerStart();
        int ServerRun();

};

#endif