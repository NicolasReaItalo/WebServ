/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi-post.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 11:58:15 by nrea              #+#    #+#             */
/*   Updated: 2024/09/25 17:09:33 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*c++ -Wall -Wextra -Werror -g3 -std=c++98 cgi-post.cpp -o poc-post*/


#include <list>
#include <vector>
#include <map>
#include <iostream>
# include <sys/types.h>
# include <sys/wait.h>
#include <sys/stat.h>
#include <cerrno>
# include <stdio.h>
# include <cstring>
#include <unistd.h>
#include <set>
#include <exception>
#include <unistd.h>
#include <stdexcept>
#include <unistd.h>

#define ERROR 1
#define GET 2
#define POST 3
#define AUTOINDEX 4
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
	std::map<std::string,std::string> envMap; //variables d'environnement a passer au script cgi
	int timestamp;
	int fd_ressource;
	int cgi_pid;
	int locationIndex;
    std::streampos readIndex;
	// class ConfigServer *configServer;
} header_infos;


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





char **MapToEnv(std::map<std::string,std::string> const & map)
{
	char **env = NULL;
	std::map<std::string,std::string>::const_iterator it;
	env = new char*[map.size() + 1];
	 int i =0;
	for (it = map.begin();it != map.end(); it++)
	{
		std::string var = it->first + "=" + it->second;
		env[i] = ft_strdup(var.c_str());
		i++;
	}
	env[i] = NULL;
	return env;
}

void freeEnv(char **env)
{
	for (int i = 0; env[i] != NULL;i++)
		delete [] env[i];
	delete [] env;
}

int execute_cgi
(std::string interpreter_path, std::string script_path, std::map<std::string,std::string> envMap, int fds[2][2])
{
	char **env = MapToEnv(envMap);
	char **cmd = new char *[3];
	cmd[0] = ft_strdup(interpreter_path.c_str());
	cmd[1] = ft_strdup(script_path.c_str());
	cmd[2] = NULL;
	close(fds[0][1]);
	dup2(fds[0][0],STDIN_FILENO);
	close(fds[0][0]);

	close(fds[1][0]);
	dup2( fds[1][1],STDOUT_FILENO);
	close(fds[1][1]);
	execve(cmd[0], cmd, env);

	freeEnv(env);
	delete [] cmd[0];
	delete [] cmd[1];
	delete [] cmd;
 	return 1;
}




int main(int argc, char const **argv, char **env)
{
	(void) argv;
	(void) argc;
	(void) env;

	std::string request_body="first_name=Nicolas&last_name=Rea\r\n";


	header_infos header;
	header.toDo = POST_CGI;
	header.ressourcePath = "./cgi-post.py";
	header.interpreterPath = "/usr/bin/python3";

	header.envMap["SERVER_SOFTWARE"]= "webserv";
	header.envMap["SERVER_NAME"]= "webserv";
	header.envMap["REQUEST_METHOD"]= "POST";
	header.envMap["PATH_INFO"]= "";
	/*PATH_INFO
	Le chemin supplémentaire du script tel que donné par le client.
	Par exemple, si le serveur héberge le script « /cgi-bin/monscript.cgi »
 	et que le client demande l'url « http://serveur.org/cgi-bin/monscript.cgi/marecherche »,
	alors PATH_INFO contiendra « marecherche ».*/
	header.envMap["SCRIPT_NAME"]= "cgi-post.py";
	header.envMap["HTTP_CONTENT_TYPE"]= "application/x-www-form-urlencoded";
	header.envMap["HTTP_CONTENT_LENGTH"]= "5";
	header.envMap["HTTP_ACCEPT"]= "*/*";
	header.envMap["HTTP_ACCEPT_LANGUAGE"]= "*/*";
	header.envMap["HTTP_USER_AGENT"]= "Mozilla";
	header.envMap["HTTP_COOKIE"]= "";



	int fds[2][2];
	pipe(fds[0]); // fds 0 webserv --> STDIN
	pipe(fds[1]); //  STDOUT ----> webserv
	int pid = fork();
	if (pid == -1)
	{
		std::cerr<<"fork"<<std::endl;
		return(1);
	}
	if (!pid)
		execute_cgi(header.interpreterPath, header.ressourcePath, header.envMap, fds);
	close(fds[1][1]);
	char buffer[100] = {'\0'};
	std::string cgi_response;
	close(fds[0][0]);
	write(fds[0][1], request_body.c_str(), request_body.size()); // voir si request_body.size() provoque des soucis d'encodages
	close(fds[0][1]);
	buffer[100] = '\0';
	while (read(fds[1][0],&buffer,99) > 0)
	{
		cgi_response = cgi_response + buffer;
		for (int i = 0; i < 100; i++)
			buffer[i] = '\0';
	}

	close(fds[1][0]);
	waitpid(pid,NULL,0);
	std::cout<<cgi_response<<std::endl;
	return 0;
}
