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
		struct pollfd fds[MAX_CLIENTS + 1];
		int nfds;
		std::string host;
		int run_server();
		int server_cleanup();
		void do_request(char *buffer, int i);
		void method_get(std::string &request, int i);
		void method_post(std::string &request, int i);
		void method_delete(std::string &request, int i);
		void fds_cleanup();
		void resp_400(std::string request, int i);
		void resp_404(std::string request, int i);

		


	public : 
		Server();
		Server(int port, std::string host);
		int start_server();
		int stop_server();
		
		
};


#endif