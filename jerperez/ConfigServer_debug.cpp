/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer_debug.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/01 13:06:52 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBlock.hpp"
#include "ConfigServer.hpp"
#include <string>
#include <iostream>

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
