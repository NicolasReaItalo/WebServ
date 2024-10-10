/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_cgi_post.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 11:36:40 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/10 12:18:15 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "cgi_utils.hpp"

char** MapToEnv(std::map<std::string,std::string> const & map);

static int execute_cgi
(std::string interpreter_path, std::string script_path, std::map<std::string,std::string> envMap, int p, int q)
{

	char **env = MapToEnv(envMap);
	char **cmd = new char *[3];

	cmd[0] = strdup(interpreter_path.c_str());
	cmd[1] = strdup(script_path.c_str());
	cmd[2] = NULL;
	execve(cmd[0], cmd, env);
	close(p);
	close (q);
	freeEnv(env);
	delete [] cmd[0];
	delete [] cmd[1];
	delete [] cmd;
	exit (0);
}

void Server::method_post_cgi(int fd, header_infos& header)
{

	std::string tmp = header.infile;
	header.infile = header.ressourcePath;
	header.ressourcePath = tmp;
	{
		std::ostringstream oss;
		oss << "[method CGI post] starting for fd " << fd;
		webservLogger.log(LVL_INFO, oss);
	}
	
	int q = open(header.infile.c_str(), O_RDWR);
	if (q == -1) {
		std::cerr << "Failed to open file: " << strerror(errno) << std::endl << header.infile << std::endl;
		return;
	}
	header.timestamp = std::time(NULL);
	std::stringstream opath;
	opath << "/tmp/tmpfileout" << &header;
	
	int tr = open(opath.str().c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (tr == -1) {
		std::cerr << "Failed to open file: " << strerror(errno) << std::endl << opath.str() << std::endl;
		return;
	}
	header.uri = opath.str();
	pid_t pid = fork();

	if (pid == -1)
	{
		std::cerr << "fork" << std::endl;
		return;
	}

	
	if (pid == 0)
	{
		std::map<int, fdsets>::iterator it;
        for (it = fd_set.begin(); it != fd_set.end();)
        {
			if (it->first != tr && it->first != q)
			{
				close (it->first);
				fd_set.erase(it->first);
				it = fd_set.begin();
			}
		}
		if (dup2(tr, STDOUT_FILENO) == -1) {
			perror("dup2 stdout");
			close(tr);
			exit(EXIT_FAILURE);
		}
		if (dup2(q, STDIN_FILENO) == -1) {
			perror("dup2 stdin");
			close(tr);
			close(q);
			exit(EXIT_FAILURE);
		}
		execute_cgi(header.interpreterPath, header.ressourcePath, header.envMap, tr, q);
		close(tr);
		close (q);
		exit (0);
	}

	
	else
	{
		header.cgi_pid = pid;
		cgiList[fd] = header;
		close(tr);
		close (q);
	}

}
