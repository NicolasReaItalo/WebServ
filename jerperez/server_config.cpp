/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_config.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 16:02:57 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/19 16:05:55 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include "DirectiveBlock.hpp"
#include "Directive.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"

#define PR_KNOWN_DIRECTIVES "autoindex alias error_page index limit listen \
cgi client_body_path client_max_body_size location root server_name"

// DEBUG
#include <iostream>
#include <iomanip>
#include <sstream>


static void	_print_dir(DirectiveBlock d)
{

	int								type = d.getType();
	//std::cout << "type:" << type << std::endl;
	Directive						*context = d.getContext();
	//std::cout << "context:" << context << std::endl;
	std::deque<std::string>			args = d.getArgs();
	std::string	context_name("");

	if (0 == context)
		context_name = "/";
	else if (0 == context->getType())
		context_name = "main";
	else
		context_name = context->getArgs().front();
	//std::cout << __FILE__ << ":" << __LINE__ << std::endl;//
	if (PR_DIR_TYPE_BLOCK == type)
		std::cout << "BLOCK:\t\t[" << std::setw(10) << context << std::setw(10) << context_name << "]\tArgs:\t";
	else if (PR_DIR_TYPE_SIMPLE == type)
		std::cout << "DIRECTIVE:\t[" <<  std::setw(10) << context << std::setw(10) << context_name << "]\tArgs:\t";
	else if (-1 == type)
		std::cout << "ERROR DIRECTIVE:\t[" <<  std::setw(10) << context << std::setw(10) << context_name << "]\tArgs:\t";
	for (std::deque<std::string>::iterator it = args.begin(); it != args.end(); ++it)
		std::cout << *it << "\t";
	std::cout << std::endl;
	if (PR_DIR_TYPE_BLOCK == type)
	{
		DirectiveBlock::instructions_t	instruction = d.getInstructions();
		for (DirectiveBlock::instructions_t::iterator it = instruction.begin(); it != instruction.end(); ++it)
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


static void	_print_error_code(const int err_code, token_deq_t::iterator	&it_curr, token_deq_t::const_iterator it_end)
{
	std::cout << "parsing: error:";
	if (it_curr != it_end)
	{
		if (-1 == it_curr->token_id)
			std::cout << " at word: `" << it_curr->word << "'";
		else
			std::cout << " at token: `" << (char)(it_curr->token_id) << "'";
	}
	std::cout << " code: " << err_code << std::endl;
}

//
int	pr_parse_config(token_deq_t &list)
{
	DirectiveBlock				context;
	token_deq_t::iterator	it_curr = list.begin();
	token_deq_t::const_iterator	it_end = list.end();
	std::list<std::string>	knownDirectives;
	int						err_code;

	context.setType(0); //
	//std::cout << __FILE__ << ":" << __LINE__ << ": context type: " << context.getType() << std::endl;//
	_pushKnownDirectives(knownDirectives);
	err_code = 0;
	while (0 == err_code && it_curr != it_end)
	{
		DirectiveBlock			next_directive;
		err_code = pr_next_directive(it_curr, it_end, &context, next_directive);
		//std::cout << __FILE__ << ":" << __LINE__ << ": context type: " << context.getType() << std::endl;//
		//std::cout << "pr_parse_config:pr_next_directive done" << (it_curr == it_end) << std::endl; //
		if (err_code)
			return (_print_error_code(err_code, it_curr, it_end), err_code); //
		//std::cout << "_print_dir" << std::endl; //
		_print_dir(next_directive); //
		ServerConfig	server;
		//std::cout << "setKnownDirectives" << std::endl; //
		server.setKnownDirectives(&knownDirectives);
		//std::cout << "_debugPlaceholder" << std::endl; //
		if (server._debugPlaceholder())
			return (1); //
		_debug_test_server(server); //
	}
	return (0);
}