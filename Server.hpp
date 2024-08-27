#ifndef SERVER_HPP
#define SERVER_HPP
#include "webserv.hpp"

class Server
{
	private :
		int server_fd;
		struct sockaddr_in address;
		int addrlen;
		int PORT;
		struct pollfd fds[MAX_CLIENTS];
		int nfds;
		void run_server();
		int server_cleanup();

	public : 
		Server();
		int start_server();
	

};


#endif