/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 15:20:58 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/18 11:12:04 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include "tokenizer.hpp"
#include "parser.hpp"
#include "Directive.hpp"
#include "Server.hpp"
#include "BlockDirective.hpp"
#include "InvalidDirective.hpp"
#include <iomanip>
#include <cstdlib>

// static void	_print_tokens(token_deq_t list)
// {
// 	token_deq_t::const_iterator	it_end = list.end();

// 	for (token_deq_t::iterator	it = list.begin(); it != it_end; ++it)
// 	{
// 		if (-1 == it->token_id)
// 			std::cout << it->word;
// 		else if (' ' == it->token_id || '\t' == it->token_id)
// 			std::cout << "\e[33m_\e[0m";
// 		else if ('\n' == it->token_id)
// 			std::cout << "\e[33m(newline)\n\e[0m";
// 		else
// 			std::cout << "\e[33m" << (char)(it->token_id) << "\e[0m";
// 	}
// 	std::cout << std::endl;
// }

static int	_parse(char *pathname, std::list<ServerConfig> &servers)
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
	// _print_tokens(list);
	std::cout << "\e[34m##############################\e[0m" << std::endl;
	if (pr_parse_config(list, servers))
	{
		std::cerr << "main: Parsing failed" << std::endl;
		return (1);
	}
	return (0);
}


int	main(int ac, char *av[])
{
	int ret = 0;
	std::list<ServerConfig> servers;

	if (2 == ac)
		ret = _parse(av[1], servers);
	else
	{
		ret = 2;
		return ret;
	}

	Server server(servers);

	server.ServerStart();

}
