/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_cgi_get.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 10:40:07 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/10 14:29:48 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "cgi_utils.hpp"

char** MapToEnv(std::map<std::string,std::string> const & map);

int execute_cgi
(std::string interpreter_path, std::string script_path, std::map<std::string,std::string> envMap, int p)
{
	
	char **env = MapToEnv(envMap);
	char **cmd = new char *[3];
	
	cmd[0] = strdup(interpreter_path.c_str());
	cmd[1] = strdup(script_path.c_str());
	cmd[2] = NULL;

	if (dup2(p, STDOUT_FILENO) == -1) {
		perror("dup2");
		close(p);
		freeEnv(env);
		delete [] cmd[0];
		delete [] cmd[1];
		delete [] cmd;
		exit(EXIT_FAILURE);
	}
	execve(cmd[0], cmd, env);
	close(p);
	freeEnv(env);
	delete [] cmd[0];
	delete [] cmd[1];
	delete [] cmd;
	return 1;
}

void Server::method_get_cgi(header_infos& header, int fd)
{
	{
		std::ostringstream oss;
		oss << "[method CGI get] starting for fd " << fd;
		webservLogger.log(LVL_INFO, oss);
	}
	header.timestamp = std::time(NULL);
	std::stringstream opath;
	opath << "/tmp/tmpfile" << &fd;
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
			if (it->first != tr)
			{
				close (it->first);
				fd_set.erase(it->first);
				it = fd_set.begin();
			}
		}
		execute_cgi(header.interpreterPath, header.ressourcePath, header.envMap, tr);
		close(tr);
		exit (0);
	}
	else
	{
		header.cgi_pid = pid;
		cgiList[fd] = header;
		close(tr);
	}
		
}