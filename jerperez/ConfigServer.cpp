/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/26 10:57:08 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigServer.hpp"
#include <string>
#include <sstream>
#include <iostream> // DEBUG
#include <limits.h>
#include <stdlib.h>
#include <algorithm>

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
	return *this;
}

//

/* Returns true if directive_name in Directives at location
 * Returns false else
 * Undefined for directive_name = ""
 * location = server.getLocation(uri)
 * Ex: `root /var/www;' -> server.inDirectives(-1, "root") -> true
 * Ex: `potato /var/www;' -> server.inDirectives(-1, "root") -> false
 */
bool		ConfigServer::inDirectives(int location, std::string directive_name)
{
	if (-1 == location)
		return ConfigBlock::inDirectives(directive_name);
	return this->_locations[location].inDirectives(directive_name);
}

/* Returns true if directive_name in Directives at location
 * Returns false else
 * Undefined for directive_name = ""
 * location = server.getLocation(uri)
 * Ex: `error_page 404 /error.html;' -> server.inDirectives(-1, "error_page") -> true
 * Ex: `error_page 404 /error.html;' -> server.inDirectives(-1, "potato") -> false
 */
bool	ConfigServer::inIODirectives(int location, std::string directive_name)
{
	if (-1 == location)
		return ConfigBlock::inIODirectives(directive_name);
	return this->_locations[location].inIODirectives(directive_name);
}

//

/* Returns first directive parameter from directive_name at location
 * location = server.getLocation(uri)
 * valid directive_name can be checked using inIODirectives(location, directive_name)
 * Ex: `root /var/www;' -> server.getDirectiveParameter(-1, "root") -> "/var/www"
 * Ex: `potato /var/www;' -> server.getDirectiveParameter(-1, "root") -> ""
 * Ex: `potato foo bar' -> server.getDirectiveParameter(-1, "potato") -> "foo"
 */
const std::string &ConfigServer::getDirectiveParameter(int location, std::string directive_name)
{
	if (-1 != location)
	{
		ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return lConfig.getDirectiveParameter(directive_name);
	}
	return ConfigBlock::getDirectiveParameter(directive_name);
}

/* Returns directive output at location
 * location = server.getLocation(uri)
 * valid directive_name can be checked using inIODirectives(location, directive_name)
 * Returns "" if directive_name is invalid or input is invalid
 * Ex: `cgi .py /python3; cgi .foo /bar;' 
 * 		-> server.getDirectiveOutput(-1, "cgi", ".py") -> "/python3"
 * 		-> server.getDirectiveOutput(-1, "cgi", ".foo") -> "/bar"
 * 		-> server.getDirectiveOutput(-1, ".foo", "/bar") -> ""
 */
const std::string	&ConfigServer::getDirectiveOutput(int location, std::string directive_name, std::string input)
{
	if (-1 != location)
	{
		ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inIODirectives(directive_name)) // 
			return lConfig.getDirectiveOutput(directive_name, input);
	}
	return ConfigBlock::getDirectiveOutput(directive_name, input);
}

/* Returns true if the parameter is among the parameters at location
 * location = server.getLocation(uri)
 * valid directive_name can be checked using inDirectives
 * Returns false if directive_name is invalid or parameter is not found
 * Undefined for directive_name = ""
 * Ex: `limit POST GET;' 
 * 		-> server.inDirectiveParameters(-1, "limit", "GET") -> true
 * 		-> server.inDirectiveParameters(-1, "potato", "POST") -> false
 * 		-> server.inDirectiveParameters(-1, "limit", "limit") -> false
 */
bool	ConfigServer::inDirectiveParameters(int location, std::string directive_name, std::string parameter)
{
	if (-1 != location)
	{
		ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return lConfig.inDirectiveParameters(directive_name, parameter);
	}
	return ConfigBlock::inDirectiveParameters(directive_name, parameter);
}

/* Returns a container with the parameters at location
 * location = server.getLocation(uri)
 * valid directive_name can be checked using inDirectives(location, directive_name)
 * Ex: `limit POST GET;' 
 * 		-> server.getDirectiveParameters(-1, "limit") -> {"POST", "GET"}
 * 		-> server.getDirectiveParameters(-1, "potato") -> {""}
 */
const ConfigServer::parameters_t	&ConfigServer::getDirectiveParameters(int location, std::string directive_name)
{
	if (-1 != location)
	{
		ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return lConfig.getDirectiveParameters(directive_name);
	}
	return ConfigBlock::getDirectiveParameters(directive_name);
}

/* Return true is the server matches the address and port
 * Ex: `listen 127.0.0.1:80;' 
 * 		-> server.serverCmp("127.0.0.1", "80") -> true
 * 		-> server.serverCmp("127.0.0.1", "81") -> false
 */
bool	ConfigServer::serverCmp(\
	const std::string &address,\
	const std::string &port) const
{
	return (address == this->_address && port == this->_port);
}

/* Return true is the server matches the address and port
 * and server_name if any
 * Ex: `listen 127.0.0.1:80; server_names foo;' 
 * 		-> server.serverCmp("127.0.0.1", "80", "") -> true
 * 		-> server.serverCmp("127.0.0.1", "80", "foo") -> true
 * 		-> server.serverCmp("127.0.0.1", "80", "bar") -> false
 */
bool	ConfigServer::serverCmp(\
	const std::string &address,\
	const std::string &port,\
	const std::string &server_name) const
{
	if (false == serverCmp(address, port))
		return false;
	if ("" == server_name)
		return true;
	const parameters_t					&server_names = this->_server_names;
	const parameters_t::const_iterator	&it_end = server_names.end();

	return (it_end != std::find(server_names.begin(), it_end, server_name));
}

//

/* Returns the full path from decoded_uri at location
 * location = server.getLocation(decoded_uri)
 * takes into account the alias ant root directives
 * with alias taking priority over root 
 * Does not check file presence / permissions
 * Ex: `root /var/www/; location /images {alias /var/www/assets/images};' 
 * 		-> server.getFullPath("/foo/bar", -1) -> "/var/www//foo/bar"
 * 		-> server.getFullPath("/images/foo.bar", loc) -> "/var/www/assets/images/foo.bar"
 */
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
	return uri;
}

//

/* Gets the location from the decoded_uri
 * Returns -1 if the directives are to be found in the server block
 * Returns a unique identifier > -1 if found in a location block
 * ex `location /var/www/ {...;}'
 * 		-> server.getLocation("/") -> -1
 * 		-> server.getLocation("/var/www/foo.bar") -> location_id
 */
int	ConfigServer::getLocation(const std::string &decoded_uri) const
{
	const std::vector<ConfigLocation>	&locations = this->_locations;

	if (locations.empty())
		return -1;
	std::vector<ConfigLocation>::const_iterator	it_end = locations.end();
	int i = 0;

	for (std::vector<ConfigLocation>::const_iterator it = locations.begin(); it_end != it; ++it)
	{
		if (it->inLocation(decoded_uri))
			return i;
		++i;
	}
	return -1;
}

//

/* Gets address
 * from `listen [address]:[port];'
 */
const std::string				&ConfigServer::getAddress(void) const
{
	return this->_address;
}

/* Gets port
 * from `listen [address]:[port];'
 */
const std::string				&ConfigServer::getPort(void) const
{
	return this->_port;
}

/* Gets the server names
 * from `server_name [...];'
 */
const ConfigServer::parameters_t	&ConfigServer::getServerNames(void) const
{
	return this->_server_names;
}

//

int		ConfigServer::_addLocation(DirectiveBlock* block_directive)
{
	DirectiveBlock::args_t							args = block_directive->getArgs();
	DirectiveBlock::args_t::iterator				it = args.begin();
	const DirectiveBlock::args_t::const_iterator	&it_end = args.end();

	if (it == it_end || "location" != *it || ++it == it_end)
		return 1;
	std::string	uri = *it;

	if ("" == uri || ++it != it_end)
		return 1;
	ConfigLocation					location(uri);

	location.setKnownDirectives(this->_knownDirectives);
	DirectiveBlock::instructions_t	instructions = block_directive->getInstructions();
	const DirectiveBlock::instructions_t::const_iterator	ite_end = instructions.end();

	for	(DirectiveBlock::instructions_t::iterator ite = instructions.begin(); ite != ite_end; ++ite)
	{
		if (PR_DIR_TYPE_SIMPLE != ite->getType())
			return 1;
		else if (location._addDirective(&*ite))
			return 1;
	}
	this->_locations.push_back(location); //vector not optimized
	return 0;
}

int	ConfigServer::addServer(Directive* unknown_directive)
{
	if (PR_DIR_TYPE_BLOCK != unknown_directive->getType())
		return CF_ERRNOTB;
	Directive::args_t					args = unknown_directive->getArgs();
	Directive::args_t::iterator			it = args.begin();
	Directive::args_t::const_iterator	it_end = args.end();

	if (it == it_end || "server" != *it || ++it != it_end)
		return CF_ERRNOTS;
	DirectiveBlock*	server_directive = reinterpret_cast<DirectiveBlock*>(unknown_directive);
	DirectiveBlock::instructions_t	instructions = server_directive->getInstructions();
	DirectiveBlock::instructions_t::const_iterator	ite_end = instructions.end();

	for	(DirectiveBlock::instructions_t::iterator ite = instructions.begin(); ite != ite_end; ++ite)
	{
		if (PR_DIR_TYPE_BLOCK == ite->getType())
		{
			if (this->_addLocation(&*ite))
				return CF_ERRNOTL;
		}
		else if (this->_addDirective(&*ite))
			return CF_ERRNOTD;
	}
	return this->_fillAll();
}

//

/* Use this to hard code a configuration of your choosing
 */
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

	// Do not remove evals
	if (this->_evalListen())
		return (1);
	this->_evalServerName();
	//
	this->_fillAll(); // COMMENT TO DISABLE DEFAULT VALUES
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

//

void	ConfigServer::_pushSplitParameters(\
	std::string name, \
	std::string joined_parameter, \
	char delimiter=' ')
{
	std::stringstream				ss(joined_parameter); 
	std::string						parameter;
	parameters_t			&parameters = this->_directive_parameters[name];
 
    while (std::getline(ss, parameter, delimiter)) 
		parameters.push_back(parameter);
}

//Evals
int	ConfigServer::_evalServerName(void)
{
	if (false == this->ConfigBlock::inDirectives("server_name"))
		return 1;
	ConfigBlock::parameters_t &nameParameter = this->_directive_parameters["server_name"];
	if (nameParameter.empty())
		return 1;
	this->_server_names = nameParameter;
	return 0;
}

int	ConfigServer::_evalListen(void)
{
	if (!this->ConfigBlock::inDirectives("listen"))
		return 1;
	parameters_t &listenParameter = this->_directive_parameters["listen"];
	if (1 != listenParameter.size())
		return CF_ERRARGS;
	std::stringstream		ss(listenParameter.front());
	std::string				elem;
	int						i = 0;
	while (std::getline(ss, elem, ':'))
	{
		if (0 == i)
			this->_address = elem;
		else if (1 == i)
			this->_port = elem;
		else
			return CF_ERRARGS;
		++i;
	}
	if (2 != i)
		return CF_ERRARGS;
	return 0;
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
	if (!this->ConfigBlock::inDirectives("index"))
		this->_pushSplitParameters("index", "GET POST DELETE");
	if (!this->ConfigBlock::inDirectives("client_body_path"))
		this->_directive_parameters["client_body_path"].push_back("/var/www/default_upload_path"); //= this->_strToParameters("/var/www/default_upload_path");
	if (!this->ConfigBlock::inDirectives("client_max_body_size"))
		this->_directive_parameters["client_max_body_size"].push_back("500000");//  = this->_strToParameters("500000");
	if (0)
	{
		std::cout << "\e[34m##############################\e[0m" << std::endl; ///
		this->_debug_print(); //CHANGE ME
	}
	return (0);
}

//

