/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_config.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 16:02:57 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/17 13:53:18 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include "BlockDirective.hpp"
#include "Directive.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"

#define PR_KNOWN_DIRECTIVES "autoindex alias error_page index limit listen \
cgi client_body_path client_max_body_size location root server_name"

// DEBUG
#include <iostream>
#include <iomanip>
#include "InvalidDirective.hpp"
#include <sstream>


static void	_print_dir(Directive *d)
{
	int								type = d->getType();
	std::deque<std::string>			args = d->getArgs();
	if (PR_DIR_TYPE_BLOCK == type)
		std::cout << "BLOCK:\t\t[" << std::setw(10) <<  d->getContext() << std::setw(10) << (reinterpret_cast<BlockDirective*>(d))->getContext()->getArgs().front() << "]\tArgs:\t";
	else if (PR_DIR_TYPE_SIMPLE == type)
		std::cout << "DIRECTIVE:\t[" <<  std::setw(10) <<  d->getContext() << std::setw(10) << (reinterpret_cast<BlockDirective*>(d))->getContext()->getArgs().front() << "]\tArgs:\t";
	else if (-1 == type)
		std::cout <<  std::setw(10) <<  (reinterpret_cast<InvalidDirective*>(d))->getWhat() <<  "[" << std::setw(10) <<  d->getContext() << std::setw(10) << (reinterpret_cast<BlockDirective*>(d))->getContext()->getArgs().front() << "]\tArgs:\t";
	for (std::deque<std::string>::iterator it = args.begin(); it != args.end(); ++it)
		std::cout << *it << "\t";
	std::cout << std::endl;
	if (PR_DIR_TYPE_BLOCK == type)
	{
		std::deque<Directive*>			instruction = (reinterpret_cast<BlockDirective*>(d))->getInstructions();
		for (std::deque<Directive*>::iterator it = instruction.begin(); it != instruction.end(); ++it)
			_print_dir(*it);
	}
}
//

// static bool	_is_server(Directive* d)
// {
// 	Directive::args_t	args = d->getArgs();

// 	//if (args.empty())
// }

static void	_pushKnownDirectives(std::list<std::string>	&knownDirectives)
{
	if (false == knownDirectives.empty())
		return ;
	std::stringstream	ss(PR_KNOWN_DIRECTIVES);
	std::string			directive_name; 

	while (std::getline(ss, directive_name, ' ')) 
		knownDirectives.push_back(directive_name);
}


static void	_debug_test_server(ServerConfig &server)
{
	std::string	uri;
	int			location;
	std::string	error_page;
	std::string	error_code;

	std::cout << "#######################################################" << std::endl;
	uri = "lol";
	std::cout << "uri=" << std::setw(20) << uri;
	location = server.getLocation(uri);
	std::cout	<< " location=" << std::setw(2) << location
				<< " fullpath=" << std::setw(20) << server.getFullPath(uri, location) << std::endl;
	uri = "/";
	std::cout << "uri=" << std::setw(20) << uri;
	location = server.getLocation(uri);
	std::cout	<< " location=" << std::setw(2) << location
				<< " fullpath=" << std::setw(20) << server.getFullPath(uri, location) << std::endl;
	uri = "/bin/bash";
	std::cout << "uri=" << std::setw(20) << uri;
	location = server.getLocation(uri);
	std::cout	<< " location=" << std::setw(2) << location
				<< " fullpath=" << std::setw(20) << server.getFullPath(uri, location) << std::endl;
	uri = "/bin//bash";
	std::cout << "uri=" << std::setw(20) << uri;
	location = server.getLocation(uri);
	std::cout	<< " location=" << std::setw(2) << location
				<< " fullpath=" << std::setw(20) << server.getFullPath(uri, location) << std::endl;

	uri = "/bin//prout";
	std::cout << "uri=" << std::setw(20) << uri;
	location = server.getLocation(uri);
	std::cout	<< " location=" << std::setw(2) << location
				<< " fullpath=" << std::setw(20) << server.getFullPath(uri, location) << std::endl;

	uri = "/bin/../";
	std::cout << "uri=" << std::setw(20) << uri;
	location = server.getLocation(uri);
	std::cout	<< " location=" << std::setw(2) << location
				<< " fullpath=" << std::setw(20) << server.getFullPath(uri, location) << std::endl;

	uri = "/bin/../../";
	std::cout << "uri=" << std::setw(20) << uri;
	location = server.getLocation(uri);
	std::cout	<< " location=" << std::setw(2) << location
				<< " fullpath=" << std::setw(20) << server.getFullPath(uri, location) << std::endl;

	uri = "/bin/../../../";
	std::cout << "uri=" << std::setw(20) << uri;
	location = server.getLocation(uri);
	std::cout	<< " location=" << std::setw(2) << location
				<< " fullpath=" << std::setw(20) << server.getFullPath(uri, location) << std::endl;

	uri = "/bin/../../../";
	std::cout << "uri=" << std::setw(20) << uri;
	location = server.getLocation(uri);
	std::cout << " location=" << std::setw(2) << location;
	error_code = "404";
	error_page = server.getDirectiveOutput(location, "error_page", error_code);
	std::cout 	<<  " error_code=" << std::setw(5) << error_code
				<<  " error_page=" << error_page << std::endl;

	uri = "/";
	std::cout << "uri=" << std::setw(20) << uri;
	location = server.getLocation(uri);
	std::cout << " location=" << std::setw(2) << location;
	error_code = "404";
	error_page = server.getDirectiveOutput(location, "error_page", error_code);
	std::cout 	<<  " error_code=" << std::setw(5) << error_code
				<<  " error_page=" << error_page << std::endl;
}

int	pr_parse_config(token_deq_t &list)
{
	Directive*				context = new BlockDirective();
	token_deq_t::iterator	it_curr = list.begin();
	Directive*				d;
	std::list<std::string>	knownDirectives;

	_pushKnownDirectives(knownDirectives);
	d = pr_next_directive(it_curr, list.end(), context);
	while (0 != d && -1 != d->getType())
	{
		_print_dir(d); //
		ServerConfig	server;
		server.setKnownDirectives(&knownDirectives);
		if (server.addServer(d))
		{
			delete d;
			delete context;
			std::cerr << "parser: failed to add server" << std::endl;
			return (1);
		}
		delete d;
		_debug_test_server(server);
		d = pr_next_directive(it_curr, list.end(), context);
	}
	if (d && -1 == d->getType())
	{
		delete d;
		delete context;
		return (1);
	}
	delete context;
	return (0);
}