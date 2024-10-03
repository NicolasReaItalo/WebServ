/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer_add.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/03 12:12:30 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBlock.hpp"
#include "ConfigServer.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <climits>
#include "config_error_constants.h"

void	ConfigServer::_pushSplitParameters(\
	std::string name, \
	std::string joined_parameter, \
	char delimiter=' ')
{
	std::stringstream		ss(joined_parameter); 
	std::string				parameter;
	parameters_t			&parameters = this->_directive_parameters[name];
 
    while (std::getline(ss, parameter, delimiter)) 
		parameters.push_back(parameter);
}

int	ConfigServer::_fillAll(void)
{
	int	err_code;

	if (INT_MAX < this->_locations.size())
		return (1);
	err_code = this->_evalListen();
	if (err_code)
		return (err_code);
	this->_evalServerName(); //
	if (!this->ConfigBlock::inDirectives("autoindex"))
		this->_directive_parameters["autoindex"].push_back("off");
	if (!this->ConfigBlock::inDirectives("root"))
		this->_directive_parameters["root"].push_back("/");
	if (!this->ConfigBlock::inDirectives("limit"))
		this->_pushSplitParameters("limit", "GET POST DELETE");
	if (!this->ConfigBlock::inDirectives("client_body_path"))
		this->_directive_parameters["client_body_path"].push_back("/var/www/default_upload_path");
	if (!this->ConfigBlock::inDirectives("client_max_body_size"))
		this->_directive_parameters["client_max_body_size"].push_back("500000");
	if (0)
		this->_debug_print(); //CHANGE ME
	return (0);
}

int		ConfigServer::_addLocation(DirectiveBlock* block_directive)
{
	DirectiveBlock::args_t							args = block_directive->getArgs();
	DirectiveBlock::args_t::const_iterator			it = args.begin();
	const DirectiveBlock::args_t::const_iterator	&it_end = args.end();

	if (it == it_end || "location" != *it || ++it == it_end)
		return CF_ERRNOTL;
	std::string	uri = *it;

	if ("" == uri || ++it != it_end)
		return CF_ERRNOURI;
	ConfigLocation					location(uri);

	location.setKnownDirectives(this->_knownDirectives);
	DirectiveBlock::instructions_t	instructions = block_directive->getInstructions();
	const DirectiveBlock::instructions_t::const_iterator	ite_end = instructions.end();
	int err_code;

	for	(DirectiveBlock::instructions_t::iterator ite = instructions.begin(); ite != ite_end; ++ite)
	{
		err_code = 0;
		if (PR_DIR_TYPE_SIMPLE != ite->getType())
			err_code = CF_ERRNOTD;
		else
			err_code = location._addDirective(&*ite);
		if (err_code)
			return (err_code);
	}
	this->_locations.push_back(location); //vector not optimized
	return 0;
}

static void	_print_error_message(Directive* unknown_directive, int err_code)
{
	if (CF_ERRNOTB == err_code)
		std::cout << "ConfigServer: error: expected block directive" << std::endl;
	else if (CF_ERRNOTS == err_code)
		std::cout << "ConfigServer: error: expected server directive" << std::endl;
	else if (CF_ERRDPARAM == err_code)
		std::cout << "ConfigServer: error: bad parameters for directive name `" << unknown_directive->getArgs().front() << "'" << std::endl;
	else if (CF_ERRDNOPARAM == err_code)
		std::cout << "ConfigServer: error: no parameters for directive name `" << unknown_directive->getArgs().front() << "'" << std::endl;
	else if (CF_ERRDUNKNOWN == err_code)
		std::cout << "ConfigServer: error: unknown directive name `" << unknown_directive->getArgs().front() << "'" << std::endl;
	else if (CF_ERRDNONAME == err_code)
		std::cout << "ConfigServer: error: missing directive name" << std::endl;
	else if (CF_ERRNOTL == err_code)
		std::cout << "ConfigServer: error: expected location" << std::endl;
	else if (CF_ERRNOTD == err_code)
		std::cout << "ConfigServer: error: expected simple directive" << std::endl;
	else if (CF_ERRNOURI == err_code)
		std::cout << "ConfigServer: error: no uri" << std::endl;
	else
		std::cout << "ConfigServer: error: code: " << err_code << std::endl;
}

int	ConfigServer::addServer(Directive* unknown_directive)
{
	if (PR_DIR_TYPE_BLOCK != unknown_directive->getType())
		return (_print_error_message(unknown_directive, CF_ERRNOTB), CF_ERRNOTB);
	Directive::args_t						args = unknown_directive->getArgs();
	Directive::args_t::const_iterator		it = args.begin();
	const Directive::args_t::const_iterator	it_end = args.end();

	if (it == it_end || "server" != *it || ++it != it_end)
		return (_print_error_message(unknown_directive, CF_ERRNOTS), CF_ERRNOTS);
	DirectiveBlock*	server_directive = reinterpret_cast<DirectiveBlock*>(unknown_directive);
	DirectiveBlock::instructions_t	instructions = server_directive->getInstructions();
	DirectiveBlock::instructions_t::const_iterator	ite_end = instructions.end();
	int	err_code;

	for	(DirectiveBlock::instructions_t::iterator ite = instructions.begin(); ite != ite_end; ++ite)
	{
		err_code = 0;
		if (PR_DIR_TYPE_BLOCK == ite->getType())
			err_code = this->_addLocation(&*ite);
		else
		 	err_code = this->_addDirective(&*ite);
		if (err_code)
			return (_print_error_message(&*ite, err_code), err_code);
	}
	err_code = this->_fillAll();
	if (err_code)
		return (_print_error_message(unknown_directive, err_code), err_code);
	return (0);
}
