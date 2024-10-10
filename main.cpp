/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 15:20:58 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/10 10:03:50 by jerperez         ###   ########.fr       */
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
	std::cout << "\r\033[1;32mCaught signal " << sig << ", stopping server...\033[0m" << std::endl;
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
	if (cf_tokenize(f, list))
	{
		std::cerr << "error: tokenizer: Failed to tokentize" << std::endl;
		f.close();
		return (1);
	}
	f.close();
	std::cout << "\e[34m##############################\e[0m" << std::endl;
	if (pr_parse(list, servers))
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
	{
		if ((ret = _parse(av[1], servers)) != 0)
			return 1;
	}
	else
		return ret;
	Server server(servers);
	server.ServerStart();
	std::cout << "\n\t\t\033[1;31m<<<  EXIT  >>>\033[0m\n\n";
	return 0;
}
