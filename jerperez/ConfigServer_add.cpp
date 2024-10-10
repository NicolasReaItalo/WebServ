/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer_add.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/10 14:04:29 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBlock.hpp"
#include "ConfigServer.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <climits>
#include "config_constants.h"

static void	_perror_parsing(Directive* unknown_directive, int err_code)
{
	std::cerr << "ConfigServer: error: ";
	if (CF_ERRNOTB == err_code)
		std::cerr << CF_ERRNOTB_MSG;
	else if (CF_ERRNOTS == err_code)
		std::cerr << CF_ERRNOTS_MSG;
	else if (CF_ERRDPARAM == err_code)
		std::cerr << CF_ERRDPARAM_MSG;
	else if (CF_ERRDBADPNUM == err_code)
		std::cerr << CF_ERRDBADPNUM_MSG;
	else if (CF_ERRDMISSING == err_code)
		std::cerr << CF_ERRDMISSING_MSG;
	else if (CF_ERRDUNKNOWN == err_code)
		std::cerr << CF_ERRDUNKNOWN_MSG;
	else if (CF_ERRDNONAME == err_code)
		std::cerr << CF_ERRDNONAME_MSG;
	else if (CF_ERRNOTL == err_code)
		std::cerr << CF_ERRNOTL_MSG;
	else if (CF_ERRNOTD == err_code)
		std::cerr << CF_ERRNOTD_MSG;
	else if (CF_ERRNOURI == err_code)
		std::cerr << CF_ERRNOURI_MSG;
	else
		std::cerr << "code: " << err_code;
	if (-1 != unknown_directive->getType())
	{
		const Directive::args_t &args = unknown_directive->getArgs();

		if (false == args.empty())
			std::cerr << " at directive: `" << args.front() << '\'';
	}
	std::cerr << std::endl;
}

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
	this->_evalServerName();
	if (!this->ConfigBlock::inDirectives("autoindex"))
		this->_directive_parameters["autoindex"].push_back(CF_DFLT_AUTOINDEX);
	if (!this->ConfigBlock::inDirectives("root"))
		this->_directive_parameters["root"].push_back(CF_DFLT_ROOT);
	if (!this->ConfigBlock::inDirectives("limit"))
		this->_pushSplitParameters("limit", CF_DFLT_LIMIT);
	if (!this->ConfigBlock::inDirectives("client_body_path"))
		this->_directive_parameters["client_body_path"].push_back(CF_DFLT_CLIENT_BODY_PATH);
	if (!this->ConfigBlock::inDirectives("client_max_body_size"))
		this->_directive_parameters["client_max_body_size"].push_back(CF_DFLT_CLIENT_MAX_BODY_SIZE);
	if (CF_DEBUG)
		this->_debug_print();
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
		if (CF_DIR_TYPE_SIMPLE != ite->getType())
			err_code = CF_ERRNOTD;
		else
			err_code = location._addDirective(&*ite);
		if (err_code)
			return (err_code);
	}
	this->_locations.push_back(location); //vector not optimized
	return 0;
}

int	ConfigServer::addServer(Directive* unknown_directive)
{
	if (CF_DIR_TYPE_BLOCK != unknown_directive->getType())
		return (_perror_parsing(unknown_directive, CF_ERRNOTB), CF_ERRNOTB);
	Directive::args_t						args = unknown_directive->getArgs();
	Directive::args_t::const_iterator		it = args.begin();
	const Directive::args_t::const_iterator	it_end = args.end();

	if (it == it_end || "server" != *it || ++it != it_end)
		return (_perror_parsing(unknown_directive, CF_ERRNOTS), CF_ERRNOTS);
	DirectiveBlock*	server_directive = reinterpret_cast<DirectiveBlock*>(unknown_directive);
	DirectiveBlock::instructions_t	instructions = server_directive->getInstructions();
	DirectiveBlock::instructions_t::const_iterator	ite_end = instructions.end();
	int	err_code;

	for	(DirectiveBlock::instructions_t::iterator ite = instructions.begin(); ite != ite_end; ++ite)
	{
		err_code = 0;
		if (CF_DIR_TYPE_BLOCK == ite->getType())
			err_code = this->_addLocation(&*ite);
		else
		 	err_code = this->_addDirective(&*ite);
		if (err_code)
			return (_perror_parsing(&*ite, err_code), err_code);
	}
	err_code = this->_fillAll();
	if (err_code)
		return (_perror_parsing(unknown_directive, err_code), err_code);
	return (0);
}
