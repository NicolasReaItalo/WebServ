/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/20 15:18:21 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigServer.hpp"
#include <string>
#include <sstream>
#include <iostream> // DEBUG
#include <limits.h>
#include <stdlib.h>
#include <algorithm>

// //DEPRECIATED
// std::string ConfigServer::getCustomErrorPage(int loc, int errorCode) {(void)loc; (void)errorCode; return ("");};//
// //DEPRECIATED END

void	ConfigServer::_pushSplitParameters(std::string name, std::string joined_parameter, char delimiter=' ')
{
	std::stringstream				ss(joined_parameter); 
	std::string						parameter;
	parameters_t			&parameters = this->_directive_parameters[name];
 
    while (std::getline(ss, parameter, delimiter)) 
		parameters.push_back(parameter);
}

int	ConfigServer::_evalServerName(void)
{
	if (!this->ConfigBlock::inDirectives("server_name"))
		return (1);
	ConfigBlock::parameters_t &nameParameter = this->_directive_parameters["server_name"];
	if (nameParameter.empty())
		return (1);
	this->_server_names = nameParameter;
	return (0);
}

int	ConfigServer::_evalListen(void)
{
	if (!this->ConfigBlock::inDirectives("listen"))
		return (1);
	parameters_t &listenParameter = this->_directive_parameters["listen"];
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

void		ConfigServer::_debug_print(void)
{
	//DEBUG
	std::cout << "Server:" << std::endl;
	this->ConfigBlock::_debug_print();
	std::vector<ConfigLocation>	&locations = this->_locations;
	std::vector<ConfigLocation>::const_iterator	it_end = locations.end();
	for (std::vector<ConfigLocation>::iterator it = locations.begin(); it_end != it; ++it)
	{
		std::cout << "location `" << it->getUri() << "'" << std::endl;
		it->_debug_print();
	}
}


int	ConfigServer::_fillAll(void)
{
	if (this->_evalListen())
		return (1);
	this->_evalServerName(); //
	if (!this->ConfigBlock::inDirectives("autoindex"))
		this->_directive_parameters["autoindex"].push_back("off");// = this->_strToParameters("off");
	if (!this->ConfigBlock::inDirectives("root"))
		this->_directive_parameters["root"].push_back("/");// = this->_strToParameters("/var/www/html/");
	// if (!this->ConfigBlock::inDirectives("alias"))
	// 	this->_directive_parameters["alias"].push_back("/var/www/html/");// = this->_strToParameters("/var/www/html/");
	if (!this->ConfigBlock::inDirectives("index"))
		this->_pushSplitParameters("index", "GET POST DELETE");
	if (!this->ConfigBlock::inDirectives("client_body_path"))
		this->_directive_parameters["client_body_path"].push_back("/var/www/default_upload_path"); //= this->_strToParameters("/var/www/default_upload_path");
	if (!this->ConfigBlock::inDirectives("client_max_body_size"))
		this->_directive_parameters["client_max_body_size"].push_back("500000");//  = this->_strToParameters("500000");
	//std::cout << "\e[34m##############################\e[0m" << std::endl; ///
	if (0)
		this->_debug_print(); //CHANGE ME
	return (0);
}

bool		ConfigServer::inDirectives(int location, std::string directive_name)
{
	if (-1 == location)
		return (ConfigBlock::inDirectives(directive_name));
	return (this->_locations[location].inDirectives(directive_name));
}

const std::string &ConfigServer::getDirectiveParameter(int location, std::string directive_name)
{
	if (-1 != location)
	{
		ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return (lConfig.getDirectiveParameter(directive_name));
	}
	return (ConfigBlock::getDirectiveParameter(directive_name));
}

const std::string	&ConfigServer::getDirectiveOutput(int location, std::string directive_name, std::string input)
{
	if (-1 != location)
	{
		ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inIODirectives(directive_name)) // 
			return (lConfig.getDirectiveOutput(directive_name, input));
	}
	return (ConfigBlock::getDirectiveOutput(directive_name, input));
}

bool	ConfigServer::inDirectiveParameters(int location, std::string directive_name, std::string parameter)
{
	if (-1 != location)
	{
		ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return (lConfig.inDirectiveParameters(directive_name, parameter));
	}
	return (ConfigBlock::inDirectiveParameters(directive_name, parameter));
}

const ConfigServer::parameters_t	&ConfigServer::getDirectiveParameters(int location, std::string directive_name)
{
	if (-1 != location)
	{
		ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return (lConfig.getDirectiveParameters(directive_name));
	}
	return (ConfigBlock::getDirectiveParameters(directive_name));
}

int		ConfigServer::_addLocation(DirectiveBlock* block_directive)
{
	DirectiveBlock::args_t					args = block_directive->getArgs();
	DirectiveBlock::args_t::iterator		it = args.begin();
	DirectiveBlock::args_t::const_iterator	it_end = args.end();

	if (it == it_end || "location" != *it || ++it == it_end)
	{
		std::cerr << "ConfigServer: incorrect location Directive" << std::endl;
		return (1);
	}
	std::string	uri = *it;

	if ("" == uri || ++it != it_end)
	{
		std::cerr << "ConfigServer: expected single valid uri, received:`" << uri << "'" << std::endl;
		return (1);
	}
	ConfigLocation					location(uri);
	location.setKnownDirectives(this->_knownDirectives);
	DirectiveBlock::instructions_t	instructions = block_directive->getInstructions();
	DirectiveBlock::instructions_t::const_iterator	ite_end = instructions.end();

	for	(DirectiveBlock::instructions_t::iterator ite = instructions.begin(); ite != ite_end; ++ite)
	{
		if (PR_DIR_TYPE_SIMPLE != ite->getType())
			return (1);
		else if (location._addDirective(&*ite))
			return (1);
	}
	this->_locations.push_back(location); //vector not optimized
	return (0);
}

int	ConfigServer::addServer(Directive* unknown_directive)
{
	if (PR_DIR_TYPE_BLOCK != unknown_directive->getType())
		return (1);
	Directive::args_t					args = unknown_directive->getArgs();
	Directive::args_t::iterator			it = args.begin();
	Directive::args_t::const_iterator	it_end = args.end();

	if (it == it_end || "server" != *it || ++it != it_end)
	{
		std::cerr << "ConfigServer: incorrect server directive" << std::endl;
		return (1);
	}
	DirectiveBlock*	server_directive = reinterpret_cast<DirectiveBlock*>(unknown_directive);
	DirectiveBlock::instructions_t	instructions = server_directive->getInstructions();
	DirectiveBlock::instructions_t::const_iterator	ite_end = instructions.end();

	for	(DirectiveBlock::instructions_t::iterator ite = instructions.begin(); ite != ite_end; ++ite)
	{
		if (PR_DIR_TYPE_BLOCK == ite->getType())
		{
			if (this->_addLocation(&*ite))
			{
				std::cerr << "ConfigServer: incorrect location" << std::endl;
				return (1);
			}
		}
		else if (this->_addDirective(&*ite))
			return (1);
	}
	return (this->_fillAll());
}

ConfigServer::ConfigServer(const ConfigServer &other)
{
	*this = other;
}

ConfigServer &ConfigServer::operator=(const ConfigServer &other)
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

const std::string				&ConfigServer::getAddress(void) const
{
	return (this->_address);
}

const std::string				&ConfigServer::getPort(void) const
{
	return (this->_port);
}

const ConfigServer::parameters_t	&ConfigServer::getServerNames(void) const
{
	return (this->_server_names);
}

int	ConfigServer::getLocation(const std::string &decoded_uri)
{
	std::vector<ConfigLocation>	&locations = this->_locations;

	if (locations.empty())
		return (-1);
	std::vector<ConfigLocation>::const_iterator	it_end = locations.end();
	int i = 0;

	for (std::vector<ConfigLocation>::iterator it = locations.begin(); it_end != it; ++it)
	{
		if (it->inLocation(decoded_uri))
			return (i);
		++i;
	}
	return (-1);
}

std::string	ConfigServer::getFullPath(const std::string &decoded_uri, int location)
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

bool	ConfigServer::serverCmp(\
	const std::string &address,\
	const std::string &port) const
{
	return (address == this->_address && port == this->_port);
}

bool	ConfigServer::serverCmp(\
	const std::string &address,\
	const std::string &port,\
	const std::string &server_name) const
{
	if (false == serverCmp(address, port))
		return (false);
	if ("" == server_name)
		return (true);
	const parameters_t					&server_names = this->_server_names;
	const parameters_t::const_iterator	&it_end = server_names.end();

	return (it_end != std::find(server_names.begin(), it_end, server_name));
}

// std::string	ConfigServer::getCustomErrorPage(int location, int errorCode)
// {
// 	if (-1 != location)
// 	{
// 		ConfigLocation	&lConfig = this->_locations[location];
// 		if (lConfig.inDirectives("error_page"))
// 			return (lConfig.getCustomErrorPage(errorCode));
// 	}
// 	return (ConfigBlock::getCustomErrorPage(errorCode));
// }

int	ConfigServer::_debugPlaceholder(int debug_input)
{
	(void)debug_input;

	//host:port (NEEDED)
	if (!this->ConfigBlock::inDirectives("listen"))
		this->_directive_parameters["listen"].push_back("127.0.0.1");

	//directives example (except cgi /error_page)
	if (!this->ConfigBlock::inDirectives("root"))
		this->_directive_parameters["root"].push_back("/");
	//io directives example (cgi /error_page)
	if (!this->ConfigBlock::inIODirectives("error_page"))
		this->_io_directive_parameters["error_page"]["404"] = "/error404.html";

	//location example uri = "/bin"
	ConfigLocation	lConfig("/bin");
	lConfig._getDirectiveParametersRef()["alias"].push_back("/usr/bin"); //use lConfig. instead of this->
	this->_locations.push_back(lConfig); //adds new location

	// MAKING SURE THERE IS A LISTEN
	if (this->_evalListen())
		return (1);

	this->_fillAll(); // COMMENT TO DISABLE DEFAULT VALUES
	return (0);
}
