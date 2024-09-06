/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:45:49 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/06 15:50:08 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <map>
#include <csignal>
#include <cerrno>
#include <vector>

#define MAX_CLIENTS 500
#define MAX_EVENTS 500
#define BUFFER_SIZE 1000

extern volatile sig_atomic_t stopper;

class Server
{
	private : 
		int port;
		std::string host;
		
		int server_fd, new_socket, epoll_fd;
		struct sockaddr_in address;
		struct epoll_event ev, events[MAX_EVENTS];
		std::map<int, std::string> errorCodeList;
		std::string d_main;
		std::string d_path;

		
		int addrlen;
		int fds[MAX_CLIENTS];
		int nfds;
	
		std::map<std::string, std::string> headerRequest;
		std::map<std::string, std::string> mimeList;

		void method_get(int client_fd, char* request);
		std::map<std::string, std::string> headerRequestParser(const std::string& request);
		void send_error(int error_code, int client_fd);
		std::vector<unsigned char> load_file(const std::string &filename) ;
		void set_error_codes();
		void set_mimeList();
		bool send_header(int client_fd, int size, std::string path);
		std::string get_mime_type(const std::string &path);
		

	public :
		Server(int port, std::string host);
		~Server(){};
		int ServerStart();
		int ServerRun();
		
		
};


#endif