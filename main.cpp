/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 15:20:58 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/19 16:05:55 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include "tokenizer.hpp"
#include "parser.hpp"
#include "Directive.hpp"
#include "DirectiveBlock.hpp"
#include <iomanip>
#include <cstdlib>

static void	_print_tokens(token_deq_t list)
{
	token_deq_t::const_iterator	it_end = list.end();

	for (token_deq_t::iterator	it = list.begin(); it != it_end; ++it)
	{
		if (-1 == it->token_id)
			std::cout << it->word;
		else if (' ' == it->token_id || '\t' == it->token_id)
			std::cout << "\e[33m_\e[0m";
		else if ('\n' == it->token_id)
			std::cout << "\e[33m(newline)\n\e[0m";
		else
			std::cout << "\e[33m" << (char)(it->token_id) << "\e[0m";
	}
	std::cout << std::endl;
}

/* example function
 * args for arguments, context for calling directive (ex: main, server, etc, ../)
 * PLACEHOLDER add virtual server object to change properties
 */
// static int	_listen(Directive::funmap_arg_t args, Directive *context)
// {
// 	Directive::funmap_arg_t::iterator	it_end = args.end();
// 	Directive::funmap_arg_t::iterator	it = args.begin();

// 	(void)context;
// 	if (it == it_end)
// 		return (1);
// 	++it;
// 	if (it == it_end || it->empty())
// 		return (2);
	
// 	const int	port = std::atoi(it->c_str());

// 	if (0 > port)
// 		return (2);
// 	++it;
// 	if (it != it_end)
// 		return (2);
// 	std::cout << "PLACEHOLDER listening to " << port << std::endl; // Do stuff here
// 	return (0);
// }

static int	_read(char *pathname)
{
	std::fstream 					f;
	token_deq_t						list;

	f.open(pathname);
	if (false == f.is_open())
	{
		std::cerr << "error: fstream: Failed to open configuration file" << std::endl;
		return (1);
	}
	if (tk_tokenize(f, list))
	{
		std::cerr << "error: tokenizer: Failed to tokentize" << std::endl;
		f.close();
		return (1);
	}
	f.close();
	_print_tokens(list);
	std::cout << "\e[34m##############################\e[0m" << std::endl;
	if (pr_parse_config(list))
	{
		std::cerr << "main: Parsing failed" << std::endl;
		return (1);
	}
	return (0);
}


int	main(int ac, char *av[])
{
	if (2 == ac)
		return (_read(av[1]));
	return (2);
}
