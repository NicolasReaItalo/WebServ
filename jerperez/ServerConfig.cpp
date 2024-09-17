/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/17 14:28:28 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include <string>
#include <sstream>
#include <iostream> // DEBUG
#include <limits.h>
#include <stdlib.h>
#include <algorithm>

// class ServerConfig : public BlockSimpleConfig
// {
// 	private:
// 		std::string										_address;
// 		std::string										_port;
// 		std::list<std::string>							_server_names;
// 		std::map<std::string, BlockSimpleConfig> 		_locations;
// 	public:
// 		bool		inDirectives(std::string, std::string);
// 		std::string getDirectiveParameter(std::string, std::string);
// 		bool		inDirectiveParameters(std::string, std::string, std::string);
// 		int			_addDirective(Directive*);
// 		bool		fillDefault(void);
// 	// 	std::string get_error_page(std::string request_uri, int error_code);
// 	// 	bool get_autoindex_status(std::string request_uri);
// 	// 	bool get_method_status(std::string request_uri, std::string method); // est-ce-que la methode est autorisee?
// 	// 	bool get_cgi_status(std::string request_uri, std::string cgi); // est-ce-que le cgi est autorisee?
// 	// 	int get_return(std::string request_uri, std::string &new_host); // renvoie -1 pas de redirection | redirection : 301 ou 302
// 	// 																  // et set la reference new_host
// 	// 	std::string get_directive( std::string request_uri, std::string directive);
// 	// 		// std::string get_full_ressource_path(std::string uri);
// 	// 		// std::string get_client_max_body_size(std::string uri);
// };

// BlockSimpleConfig::directive_parameters_t	&ServerConfig::_strToParameters(std::string parameter0)
// {
// 	directive_parameters_t	parameters;

// 	parameters.push_back(parameter0);
// 	return (parameters);
// }

// BlockSimpleConfig::directive_parameters_t	&ServerConfig::_strToParameters(std::string joined_parameter, char delimiter)
// {
// 	directive_parameters_t	parameters;
// 	std::stringstream		ss(joined_parameter); 
// 	std::string				parameter;
 
//     while (std::getline(ss, parameter, ' ')) 
// 		parameters.push_back(parameter);
// 	return (parameters);
// }

void	ServerConfig::_pushSplitParameters(std::string name, std::string joined_parameter, char delimiter=' ')
{
	std::stringstream				ss(joined_parameter); 
	std::string						parameter;
	directive_parameters_t			&parameters = this->_directive_parameters[name];
 
    while (std::getline(ss, parameter, delimiter)) 
		parameters.push_back(parameter);
}

int	ServerConfig::_evalListen(void)
{
	if (!this->BlockSimpleConfig::inDirectives("listen"))
		return (1);
	directive_parameters_t listenParameter = this->_directive_parameters["listen"];
	if (1 != listenParameter.size())
		return (1);
	std::stringstream		ss(listenParameter.front());
	std::string				elem;
	int						i = 0;
	while (std::getline(ss, elem, ':'))
	{
		if (elem.empty())
			return (1); // simple check
		if (0 == i)
			this->_address = elem; //TODO: check here valid
		else if (1 == i)
			this->_port = elem; //TODO: check here valid
		else
			return (1);
		++i;
	}
	if (1 == i)
		this->_port = "80";
	else if (2 == i)
		;
	else
		return (1);
	return (0);
}

void		ServerConfig::_debug_print(void)
{
	//DEBUG
	std::cout << "Server:" << std::endl;
	this->BlockSimpleConfig::_debug_print();
	std::vector<LocationConfig>	&locations = this->_locations;
	std::vector<LocationConfig>::const_iterator	it_end = locations.end();
	for (std::vector<LocationConfig>::iterator it = locations.begin(); it_end != it; ++it)
	{
		std::cout << "location `" << it->getUri() << "'" << std::endl;
		it->_debug_print();
	}
}


int	ServerConfig::_fillAll(void)
{
	if (this->_evalListen())
		return (1);
	if (!this->BlockSimpleConfig::inDirectives("autoindex"))
		this->_directive_parameters["autoindex"].push_back("off");// = this->_strToParameters("off");
	if (!this->BlockSimpleConfig::inDirectives("root"))
		this->_directive_parameters["root"].push_back("/");// = this->_strToParameters("/var/www/html/");
	// if (!this->BlockSimpleConfig::inDirectives("alias"))
	// 	this->_directive_parameters["alias"].push_back("/var/www/html/");// = this->_strToParameters("/var/www/html/");
	if (!this->BlockSimpleConfig::inDirectives("index"))
		this->_pushSplitParameters("index", "GET POST DELETE");
	if (!this->BlockSimpleConfig::inDirectives("client_body_path"))
		this->_directive_parameters["client_body_path"].push_back("/var/www/default_upload_path"); //= this->_strToParameters("/var/www/default_upload_path");
	if (!this->BlockSimpleConfig::inDirectives("client_max_body_size"))
		this->_directive_parameters["client_max_body_size"].push_back("500000");//  = this->_strToParameters("500000");
	std::cout << "\e[34m##############################\e[0m" << std::endl; ///
	this->_debug_print();
	return (0);
}

bool		ServerConfig::inDirectives(int location, std::string directive_name)
{
	if (-1 == location)
		return (BlockSimpleConfig::inDirectives(directive_name));
	return (this->_locations[location].inDirectives(directive_name));
}

std::string ServerConfig::getDirectiveParameter(int location, std::string directive_name)
{
	if (-1 != location)
	{
		LocationConfig	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return (lConfig.getDirectiveParameter(directive_name));
	}
	return (BlockSimpleConfig::getDirectiveParameter(directive_name));
}

std::string	ServerConfig::getDirectiveOutput(int location, std::string directive_name, std::string input)
{
	if (-1 != location)
	{
		LocationConfig	&lConfig = this->_locations[location];
		if (lConfig.inIODirectives(directive_name)) // 
			return (lConfig.getDirectiveOutput(directive_name, input));
	}
	return (BlockSimpleConfig::getDirectiveOutput(directive_name, input));
}

bool	ServerConfig::inDirectiveParameters(int location, std::string directive_name, std::string parameter)
{
	if (-1 != location)
	{
		LocationConfig	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return (lConfig.inDirectiveParameters(directive_name, parameter));
	}
	return (BlockSimpleConfig::inDirectiveParameters(directive_name, parameter));
}

ServerConfig::directive_parameters_t	ServerConfig::getDirectiveParameters(int location, std::string directive_name)
{
	if (-1 != location)
	{
		LocationConfig	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return (lConfig.getDirectiveParameters(directive_name));
	}
	return (BlockSimpleConfig::getDirectiveParameters(directive_name));
}


// LocationConfig	&ServerConfig::_returnLocation(const std::string &uri)
// {
// 	LocationConfig	location(uri);
// 	location.setKnownDirectives(this->_knownDirectives);

// 	return (location);
// }

int		ServerConfig::_addLocation(BlockDirective* block_directive)
{
	BlockDirective::args_t					args = block_directive->getArgs();
	BlockDirective::args_t::iterator		it = args.begin();
	BlockDirective::args_t::const_iterator	it_end = args.end();

	if (it == it_end || "location" != *it || ++it == it_end)
	{
		std::cerr << "ServerConfig: incorrect location Directive" << std::endl;
		return (1);
	}
	std::string	uri = *it;

	if ("" == uri || ++it != it_end)
	{
		std::cerr << "ServerConfig: expected single valid uri, received:`" << uri << "'" << std::endl;
		return (1);
	}
	LocationConfig					location(uri);
	location.setKnownDirectives(this->_knownDirectives);
	BlockDirective::instructions_t	instructions = block_directive->getInstructions();
	BlockDirective::instructions_t::const_iterator	ite_end = instructions.end();

	for	(BlockDirective::instructions_t::iterator ite = instructions.begin(); ite != ite_end; ++ite)
	{
		if (PR_DIR_TYPE_SIMPLE != (*ite)->getType())
			return (1);
		else if (location._addDirective(*ite))
			return (1);
	}
	this->_locations.push_back(location); //vector not optimized
	return (0);
}

int	ServerConfig::addServer(Directive* unknown_directive)
{
	if (PR_DIR_TYPE_BLOCK != unknown_directive->getType())
		return (1);
	Directive::args_t					args = unknown_directive->getArgs();
	Directive::args_t::iterator			it = args.begin();
	Directive::args_t::const_iterator	it_end = args.end();

	if (it == it_end || "server" != *it || ++it != it_end)
	{
		std::cerr << "ServerConfig: incorrect server directive" << std::endl;
		return (1);
	}
	BlockDirective*	server_directive = reinterpret_cast<BlockDirective*>(unknown_directive);
	BlockDirective::instructions_t	instructions = server_directive->getInstructions();
	BlockDirective::instructions_t::const_iterator	ite_end = instructions.end();

	for	(BlockDirective::instructions_t::iterator ite = instructions.begin(); ite != ite_end; ++ite)
	{
		if (PR_DIR_TYPE_BLOCK == (*ite)->getType())
		{
			if (this->_addLocation(reinterpret_cast<BlockDirective*>(*ite)))
			{
				std::cerr << "ServerConfig: incorrect location" << std::endl;
				return (1);
			}
		}
		else if (_addDirective(*ite))
			return (1);
	}
	return (this->_fillAll());
}

ServerConfig::ServerConfig(const ServerConfig &other)
{
	*this = other;
}

ServerConfig &ServerConfig::operator=(const ServerConfig &other)
{
	if (&other != this)
	{
		this->_address = other._address;
		this->_directive_parameters = other._directive_parameters;
		this->_locations = other._locations;
		this->_port = other._port;
		this->_server_names = other._server_names;
	}
	return (*this);
}

const std::string				&ServerConfig::getAddress(void) const
{
	return (this->_address);
}

const std::string				&ServerConfig::getPort(void) const
{
	return (this->_port);
}

const std::list<std::string>	&ServerConfig::getServerNames(void) const
{
	return (this->_server_names);
}

int	ServerConfig::getLocation(const std::string &decoded_uri)
{
	std::vector<LocationConfig>	&locations = this->_locations;

	if (locations.empty())
		return (-1);
	std::vector<LocationConfig>::const_iterator	it_end = locations.end();
	int i = 0;

	for (std::vector<LocationConfig>::iterator it = locations.begin(); it_end != it; ++it)
	{
		if (it->inLocation(decoded_uri))
			return (i);
		++i;
	}
	return (-1);
}

std::string	ServerConfig::getFullPath(const std::string &decoded_uri, int location)
{

	std::string		uri(decoded_uri);
	std::string		alias = this->getDirectiveParameter(location, "alias");
	if ("" == alias)
	{
		std::string		root = this->getDirectiveParameter(location, "root");
		if ("" != root)
			uri = root + uri;
	}
	else
		uri.replace(0, this->_locations[location].getUri().length(), alias);
	char	*path = realpath(uri.c_str(), NULL);

	if (NULL == path)
		return ("");
	std::string	ret_path(path);

	free(path);
	return (ret_path);
}

bool	ServerConfig::serverCmp(std::string address, std::string port, std::string server_name)
{
	if (address != this->_address || port != this->_port)
		return (false);
	if ("" == server_name)
		return (true);
	std::list<std::string>				&server_names = this->_server_names;
	std::list<std::string>::iterator	it_end = server_names.end();
	return (it_end != std::find(server_names.begin(), it_end, server_name));
}

std::string	ServerConfig::getCustomErrorPage(int location, int errorCode)
{
	if (-1 != location)
	{
		LocationConfig	&lConfig = this->_locations[location];
		if (lConfig.inDirectives("error_page"))
			return (lConfig.getCustomErrorPage(errorCode));
	}
	return (BlockSimpleConfig::getCustomErrorPage(errorCode));
}

int	ServerConfig::_debugPlaceholder(int debug_input)
{
	(void)debug_input;

	//host:port (NEEDED)
	if (!this->BlockSimpleConfig::inDirectives("listen"))
		this->_directive_parameters["listen"].push_back("127.0.0.1");

	//directives example (except cgi /error_page)
	if (!this->BlockSimpleConfig::inDirectives("root"))
		this->_directive_parameters["root"].push_back("/");
	//io directives example (cgi /error_page)
	if (!this->BlockSimpleConfig::inIODirectives("error_page"))
		this->_io_directive_parameters["error_page"]["404"] = "/error404.html";

	//location example uri = "/bin"
	LocationConfig	lConfig("/bin");
	lConfig._directive_parameters["alias"].push_back("/usr/bin"); //use lConfig. instead of this->
	this->_locations.push_back(lConfig); //adds new location

	// MAKING SURE THERE IS A LISTEN
	if (this->_evalListen())
		return (1);

	this->_fillAll(); // COMMENT TO DISABLE DEFAULT VALUES
	return (0);
}
