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
		int run_server();
		int server_cleanup();
		void do_request(char *buffer, int i);
		void method_get(std::string &request, int i);
		void method_post(std::string &request, int i);
		void method_delete(std::string &request, int i);
		void fds_cleanup();


	public : 
		Server();
		int start_server();
};


#endif