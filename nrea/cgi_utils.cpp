/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_utils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 15:42:22 by nrea              #+#    #+#             */
/*   Updated: 2024/09/30 17:18:46 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi_utils.hpp"

static size_t	cpp_strlen(const char *s)
{
	unsigned int long	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}


static char	*cpp_strdup(const char *s)
{
	size_t	s_size;
	char	*dup;
	int		i;

	i = 0;
	s_size = cpp_strlen(s);
	dup = new char [s_size +1];
	while (s[i])
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

/*
Convert a std::map<std::string, std::string> to a char** null terminated array
usable in execve
/!\ After use the array must be deleted with freeEnv()
*/
char **MapToEnv(std::map<std::string,std::string> const & map)
{
	char **env = NULL;
	std::map<std::string,std::string>::const_iterator it;
	env = new char*[map.size() + 1];
	 int i =0;
	for (it = map.begin();it != map.end(); it++)
	{
		std::string var = it->first + "=" + it->second;
		env[i] = cpp_strdup(var.c_str());
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

//detect if the uri contains a cgi extension and returns it
// returns a empty string if no cgi exetension has been found
// We considere that at this stage the query_string starting
// with ? has already been removed
// /cgi-bin/my_script.cgi/path/to/resource
std::string	detect_cgi
(std::string const &uri, std::vector<std::string> const &cgi_ext)
{
	std::vector<std::string>::const_iterator it;

	for (it = cgi_ext.begin(); it != cgi_ext.end(); it++)
	{
		std::size_t ext = uri.rfind(*it);
		if (ext != std::string::npos)
			return uri.substr(ext, it->size());
	}
	return "";
}

//strip the PATH_INFO from the uri
void stripPathInfo
(std::map<std::string,std::string> &headerAttributes, std::string cgi)
{
	std::string uri = headerAttributes["URI"];
	std::size_t extension = uri.rfind(cgi);

	if (extension == std::string::npos)
		return;
	extension += cgi.size();
	if (extension <= uri.size())
	{
		std::string path_info = uri.substr(extension, uri.size());
		headerAttributes["URI"] = uri.substr(0,extension);
		headerAttributes["PATH_INFO"] = path_info;
	}
}

// int main()
// {
// 	std::vector<std::string> cgi;

// 	cgi.push_back(".cgi");
// 	cgi.push_back(".py");
// 	cgi.push_back(".php");
// 	std::cout<< detect_cgi("cgi-bin/my_script.py/path.py/to/resource",cgi)<<std::endl;
// 	std::map<std::string,std::string> attr;
// 	attr["URI"] = "cgi-bin/my_script.py/path.py/info";
// 	stripPathInfo(attr, ".py");
// 	std::cout<< attr["URI"]<<std::endl;
// 	std::cout<< attr["PATH_INFO"]<<std::endl;


// 	return 0;
// }
