/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:46 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/19 18:37:41 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <list>
#include <vector>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ServerConfig.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <set>
#include <utility>
#include <vector>

#define MAX_EVENTS 500
#define BUFFER_SIZE 100000
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
	class ServerConfig *configServer;
    std::streampos readIndex;

} header_infos;

class Server
{
    private :

        int list_size;
        std::list<ServerConfig> servers;
        std::vector<int> server_fd;
        int epoll_fd, new_socket;
        int nfds;
        std::map<int, std::pair<std::string, std::string> > fd_set;
        std::vector<struct sockaddr_in> address;
        std::vector<struct epoll_event> ev;
        struct epoll_event events[MAX_EVENTS];

        std::map<std::string, std::string> mimeList;
        std::map<int, std::string> errorList;
        std::map<int, header_infos> chunk;
        unsigned long maxBodySize;

        void closeAllFd();
        void set_mimeList();
        void set_errorList();
        /*on an event pollin*/
        void receive_data(int fd, int i);

        void method_get(header_infos header, int fd, int i);
        void method_post(header_infos header, std::vector<unsigned char> body, int fd, int i);
        void method_delete(header_infos header, int fd);
        
        std::string get_mime_type(const std::string &uri);
        header_infos headerParser(std::string rawBuffer, std::pair<std::string, std::string> address);

        void chunked_post(int fd, std::string tmp);
        void send_chunk(int fd, int i, header_infos header);
        void send_chunk(int fd, int i);

        bool is_fd_in_chunklist(int fd);
        void sendError(int errcode, int fd, header_infos header);
        std::string generate_error_page(int errcode);
        std::vector<unsigned char> load_file(const std::string &filename);
        void failed_to_send(int fd);

    public :
        Server(std::list<ServerConfig> servers);
        int ServerStart();
        int ServerRun();

};

#endif