/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:46 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/17 11:02:18 by qgiraux          ###   ########.fr       */
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
#define BUFFER_SIZE 1000000
#define CHUNK_SIZE 1000000
#define TIMEOUT 5

#define ERROR 1
#define GET 2
#define POST 3
#define AUTOINDEX 4
#define RETURN 5
#define DELETE 6
#define GET_CGI 7
#define POST_CGI 8

/*structure used to stock all information gotten fron a request header or the associated server configuration*/
typedef struct s_header_infos
{
	int toDo;
    std::string uri;
    std::string infile;
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
    std::string cookie;
} header_infos;

/*all important infos on each open fds*/
typedef struct s_fdsets
{
    std::string address;
    std::string port;
    time_t         timer;
    bool        listener;
    bool        client;
} fdsets;

/*variable to deal with SIGINT*/
extern volatile sig_atomic_t stopper;
/*adding the logger to all functions*/
extern Logger webservLogger;

class Server
{
    private :
        int tmpfindex;
        /*the number of servers*/
        int list_size;
        /*the list of all the parsed servers fron the .conf file*/
        std::list<ConfigServer> servers;
        /*vector of the listening sockets, on which new connexions arrive*/
        std::vector<int> server_fd;
        int epoll_fd, new_socket;
        /* number of fds loaded in epoll*/
        int nfds;
        /*vectors used to set the sockets, and to listen on them*/
        std::vector<struct sockaddr_in> address;
        std::vector<struct epoll_event> ev;
        /*structure that stocks all the events received by epoll, that need to be dealt with*/
        struct epoll_event events[MAX_EVENTS];
        /*on each loop, gets the time*/
        time_t time;
        /*list of open fds*/
        std::map<int, fdsets> fd_set;
        /*list of allowed mimes, and their associated extention*/
        std::map<std::string, std::string> mimeList;
        /*list of error codes, and their associated messages*/
        std::map<int, std::string> errorList;
        /*list of fds on which a chunked request is being done, and their assouciated header*/
        std::map<int, header_infos> chunk;
        /*list of fds on which a cgi isbeing done, and their assouciated header*/
        std::map<int, header_infos> cgiList;
        /*size set by the BUFFER_SIZE, for the buffer*/
        unsigned long maxBodySize;

        /*required to start or stop the server*/
        void set_mimeList();
        void set_errorList();
        void closeAllFd();
        void killAllChildren();
        
        /*on loops*/
        int ServerRun();
        void receive_data(int fd, int i);

        /*methods that can be called by a header-request*/
        void method_get(const header_infos& header, int fd);
        void method_get_cgi(header_infos& header, int fd);
        void method_post_cgi(int fd, header_infos& header);
        void method_post(header_infos& header, std::vector<unsigned char> body, int fd);
        void method_delete(const header_infos& header, int fd);
        void method_error(const header_infos& header, int fd);
        void method_autoindex(const header_infos& header, int fd);
        void method_return(const header_infos& header, int fd);

        /*special cases to modify behaviour of methods*/
        void chunked_post(int fd, std::vector<unsigned char> body, header_infos& header);
        void send_chunk(int fd, const header_infos& header);
        void send_chunk(int fd);
        void send_index(int fd, const header_infos& header, std::map<std::string, std::string>& index);
        void sendError(header_infos header, int errcode, int fd);
        void sendCustomError(header_infos header, int errcode, int fd);

        /*utils used mostly to parse header*/
        std::string get_mime_type(const std::string &uri);
        header_infos headerParser(std::string rawBuffer, std::pair<std::string, std::string> interface);
		ConfigServer * findServer(std::pair<std::string, std::string> interface, std::string host);
		header_infos handle_get(header_infos &response, ConfigServer  * config,int locationIndex,std::map<std::string, std::string> &header_attributes);
		header_infos serve_regular_file(header_infos &response, ConfigServer  * config,int locationIndex,std::map<std::string, std::string> header_attributes);
		header_infos handle_post(header_infos &response, ConfigServer  * config,int locationIndex,std::map<std::string, std::string> header_attributes);
		header_infos handle_delete(header_infos &response, ConfigServer  * config,int locationIndex,std::map<std::string, std::string> header_attributes);
		header_infos handle_dir(header_infos &response,ConfigServer  * config,int locationIndex,std::map<std::string, std::string> &header_attributes);

        /*utils used mostly to run server*/
        bool is_fd_in_chunklist(int fd);
        std::string generate_error_page(int errcode);
        std::string generate_index_page(std::map<std::string, std::string> index, const header_infos& header);
        std::vector<unsigned char> load_file(const std::string &filename);
        void failed_to_send(int fd);
        int parse_cgi_tmp_file(header_infos& header);

        bool is_socket_open(int fd);
        
    public :
        Server(std::list<ConfigServer> servers);
        int ServerStart();
        void ServerClose();
        ~Server();
        


};

#endif
