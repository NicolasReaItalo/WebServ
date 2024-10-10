/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 16:02:57 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/10 11:53:30 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigServer.hpp"
#include "DirectiveBlock.hpp"
#include "Directive.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "config_constants.h"
#include <iostream>
#include <iomanip>
#include <sstream>

static void	_print_dir(DirectiveBlock d)
{

	int								type = d.getType();
	Directive						*context = d.getContext();
	std::deque<std::string>			args = d.getArgs();
	std::string	context_name("");

	if (0 == context)
		context_name = "/";
	else if (0 == context->getType())
		context_name = "main";
	else
		context_name = context->getArgs().front();
	if (CF_DIR_TYPE_BLOCK == type)
		std::cout << "BLOCK:\t\t[" << std::setw(10) << context << std::setw(10) << context_name << "]\tArgs:\t";
	else if (CF_DIR_TYPE_SIMPLE == type)
		std::cout << "DIRECTIVE:\t[" <<  std::setw(10) << context << std::setw(10) << context_name << "]\tArgs:\t";
	else if (-1 == type)
		std::cout << "ERROR DIRECTIVE:\t[" <<  std::setw(10) << context << std::setw(10) << context_name << "]\tArgs:\t";
	for (std::deque<std::string>::iterator it = args.begin(); it != args.end(); ++it)
		std::cout << *it << "\t";
	std::cout << std::endl;
	if (CF_DIR_TYPE_BLOCK == type)
	{
		DirectiveBlock::instructions_t	instruction = d.getInstructions();
		for (DirectiveBlock::instructions_t::iterator it = instruction.begin(); it != instruction.end(); ++it)
			_print_dir(*it);
	}
}

//

static void	_pushKnownDirectives(ConfigBlock::parameters_t	&knownDirectives)
{
	if (false == knownDirectives.empty())
		return ;
	std::stringstream	ss(CF_KNOWN_DIRECTIVES);
	std::string			directive_name;

	while (std::getline(ss, directive_name, ' '))
		knownDirectives.push_back(directive_name);
}

static void	_print_error_code(\
	const int err_code, \
	token_deq_t::iterator &it_curr, \
	const token_deq_t::const_iterator it_end)
{
	std::cerr << "parsing: error: ";
	if (it_curr != it_end)
	{
		if (-1 == it_curr->token_id)
			std::cerr << "at word: `" << it_curr->word;
		else
			std::cerr << "at token: `" << (char)(it_curr->token_id);
	}
	std::cerr << "' code: " << err_code << std::endl;
}

//
/* Parse token list
 * Fills servers with ConfigServer objects
 * Returns 0 iff OK
 */
int	pr_parse(token_deq_t &list, std::list<ConfigServer> &servers)
{
	DirectiveBlock						context;
	token_deq_t::iterator				it_curr = list.begin();
	const token_deq_t::const_iterator	&it_end = list.end();
	ConfigBlock::parameters_t			knownDirectives;
	int									err_code;

	context.setType(0);
	_pushKnownDirectives(knownDirectives);
	err_code = 0;
	while (0 == err_code && it_curr != it_end)
	{
		DirectiveBlock			next_directive;
		err_code = pr_next_directive(it_curr, it_end, &context, next_directive);
		if (err_code)
		{
			_print_error_code(err_code, it_curr, it_end);
			return (err_code);
		}
		if (CF_DEBUG)
			_print_dir(next_directive);
		ConfigServer	server;
		server.setKnownDirectives(&knownDirectives);
		err_code = server.addServer(&next_directive);
		if (err_code)
			return err_code;
		servers.push_back(server);
	}
	if (servers.empty())
	{
		std::cerr << "parse: error: " << CF_ERRNOTHING_MSG << std::endl;
		return (CF_ERRNOTHING);
	}
	return 0;
}
