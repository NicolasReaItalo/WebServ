/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:46 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/08 12:09:45 by qgiraux          ###   ########.fr       */
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
#include "ConfigServer.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <set>
#include <utility>
#include <vector>
#include <csignal>
#include <ctime>
#include "Logger.hpp"
#include "http_errors.hpp"

#define MAX_EVENTS 500
#define BUFFER_SIZE 10000
#define CHUNK_SIZE 10000
#define TIMEOUT 5

#define ERROR 1
#define GET 2
#define POST 3
#define AUTOINDEX 4
#define RETURN 5
#define DELETE 6
#define GET_CGI 7
#define POST_CGI 8

typedef struct s_header_infos
{
	int toDo;
    std::string uri;
	unsigned long bodySize;
	std::string ressourcePath;
	std::string contentType;
	std::string interpreterPath;
	int returnCode;
	bool keepAlive;
	bool chunked;
	std::map<std::string,std::string> envMap;
	int timestamp;
	int fd_ressource;
	pid_t cgi_pid;
	int locationIndex;
    std::streampos readIndex;
	class ConfigServer *configServer;
    int i_ev;
} header_infos;

typedef struct s_fdsets
{
    std::string address;
    std::string port;
    time_t         timer;
    bool        listener;
    bool        client;
} fdsets;

extern volatile sig_atomic_t stopper;
extern Logger webservLogger;

class Server
{
    private :
        int list_size;
        std::list<ConfigServer> servers;
        std::vector<int> server_fd;
        int epoll_fd, new_socket;
        int nfds;
        std::map<int, fdsets> fd_set;
        std::vector<struct sockaddr_in> address;
        std::vector<struct epoll_event> ev;
        struct epoll_event events[MAX_EVENTS];
        time_t time;

        std::map<std::string, std::string> mimeList;
        std::map<int, std::string> errorList;
        std::map<int, header_infos> chunk;
        std::map<int, header_infos> cgiList;
        unsigned long maxBodySize;


        void set_mimeList();
        void set_errorList();

        /*on an event pollin*/
        void receive_data(int fd, int i);
        void closeAllFd();
        void killAllChildren();

        void method_get(const header_infos& header, int fd, int i);
        void method_get_cgi(header_infos& header, int fd, int i);
        void method_post_cgi(int fd, header_infos& header);
        void method_post(header_infos& header, std::vector<unsigned char> body, int fd, int i);
        void method_delete(const header_infos& header, int fd, int i);
        void method_error(const header_infos& header, int fd, int i);
        void method_autoindex(const header_infos& header, int fd, int i);
        void method_return(const header_infos& header, int fd);

        std::string get_mime_type(const std::string &uri);
        header_infos headerParser(std::string rawBuffer, std::pair<std::string, std::string> interface);
		ConfigServer * findServer(std::pair<std::string, std::string> interface, std::string host);
		header_infos handle_get(header_infos &response, ConfigServer  * config,int locationIndex,std::map<std::string, std::string> &header_attributes);
		header_infos serve_regular_file(header_infos &response, ConfigServer  * config,int locationIndex,std::map<std::string, std::string> header_attributes);
		header_infos handle_post(header_infos &response, ConfigServer  * config,int locationIndex,std::map<std::string, std::string> header_attributes);
		header_infos handle_delete(header_infos &response, ConfigServer  * config,int locationIndex,std::map<std::string, std::string> header_attributes);
		header_infos handle_dir(header_infos &response,ConfigServer  * config,int locationIndex,std::map<std::string, std::string> &header_attributes);

        void cgi_send(const header_infos& header, int fd, int i);
        void chunked_post(int fd, std::vector<unsigned char> body, header_infos& header);
        void send_chunk(int fd, int i, const header_infos& header);
        void send_chunk(int fd, int i);
        void send_index(int fd, const header_infos& header, std::map<std::string, std::string>& index);
        void sendError(header_infos header, int errcode, int fd, int i);
        void sendCustomError(header_infos header, int errcode, int fd, int i);

        bool is_fd_in_chunklist(int fd);
        std::string generate_error_page(int errcode);
        std::string generate_index_page(std::map<std::string, std::string> index, const header_infos& header);
        std::vector<unsigned char> load_file(const std::string &filename);
        void failed_to_send(int fd);
        int parse_cgi_tmp_file(header_infos& header);
        
    public :
        Server(std::list<ConfigServer> servers);
        int ServerStart();
        int ServerRun();
        void ServerClose();
        ~Server();


};

#endif
