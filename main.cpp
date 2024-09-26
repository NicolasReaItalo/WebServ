/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 15:20:58 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/26 11:09:54 by jerperez         ###   ########.fr       */
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
#include "Server.hpp"

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
	if (2 == ac)
		ret = _parse(av[1], servers);
	else
		return 2;
	Server server(servers);
	server.ServerStart();
	std::cout << "<<<EXIT>>>\n";
	return 0;

}
