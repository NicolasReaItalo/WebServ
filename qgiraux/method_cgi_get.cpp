/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method_cgi_get.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 10:40:07 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/02 16:52:29 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "cgi_utils.hpp"

char** MapToEnv(std::map<std::string,std::string> const & map);
size_t	ft_strlen(const char *s);
char	*ft_strdup(const char *s);

int execute_cgi
(std::string interpreter_path, std::string script_path, std::map<std::string,std::string> envMap, int p)
{
	
	char **env = MapToEnv(envMap);
	char **cmd = new char *[3];
	
	interpreter_path.copy(cmd[0], interpreter_path.length(), 0);
	script_path.copy(cmd[1], script_path.length(), 0);

	cmd[2] = NULL;
	if (dup2(p, STDOUT_FILENO) == -1) {
		perror("dup2");
		close(p);
		freeEnv(env);
		delete [] cmd;
		exit(EXIT_FAILURE);
	}
	execve(cmd[0], cmd, env);
	close(p);
	freeEnv(env);
	delete [] cmd;
 	return 1;
}

void Server::method_get_cgi(header_infos& header, int fd, int i)
{
	(void)i;
	{
		std::ostringstream oss;
		oss << "[method CGI get] starting for fd " << fd;
		webservLogger.log(LVL_INFO, oss);
	}
	header.timestamp = std::time(NULL);
	std::stringstream opath;
	opath << "/tmp/tmpfile" << &header;
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
		execute_cgi(header.interpreterPath, header.ressourcePath, header.envMap, tr);
		close(tr);
	}
	else
	{
		header.cgi_pid = pid;
		// close(header.cgi_pid);
		cgiList[fd] = header;
		close(tr);
	}
		
}






size_t	ft_strlen(const char *s)
{
	unsigned int long	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}


char	*ft_strdup(const char *s)
{
	size_t	s_size;
	char	*dup;
	int		i;

	i = 0;
	s_size = ft_strlen(s);
	dup = new char [ s_size +1];
	while (s[i])
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}