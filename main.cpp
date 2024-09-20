/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 15:20:58 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/20 17:30:53 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <iostream>
#include <fstream>
#include "tokenizer.hpp"
#include "parser.hpp"
#include "Directive.hpp"
#include "DirectiveBlock.hpp"
#include <iomanip>
#include <cstdlib>
#include "Server.hpp"
#include <csignal>



volatile sig_atomic_t stopper = 0;

void handle_sigint(int sig) {
    std::cout << "Caught signal " << sig << ", stopping server..." << std::endl;
    stopper = 1;
}

static int	_parse(char *pathname, std::list<ConfigServer> &servers)
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
	std::list<ConfigServer> servers;

	signal(SIGINT, handle_sigint);
	if (2 == ac)
		ret = _parse(av[1], servers);
	else
		return 2;
	Server server(servers);
	server.ServerStart();
	std::cout << "<<<EXIT>>>\n";
	return 0;
}
