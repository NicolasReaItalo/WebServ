/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBlock.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/19 16:05:55 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <list>
#include <map>
#include <algorithm>
#include "ConfigBlock.hpp"
#include "DirectiveBlock.hpp"
#include "Directive.hpp"
#include <iostream>

#define CF_ERRUNKNDIR 10
#define CF_ERRNODIR 11
#define CF_ERRNOPARAMS 12

ConfigBlock::ConfigBlock(void) : _knownDirectives(0) {}

ConfigBlock::ConfigBlock(const ConfigBlock &other)
{
	*this = other;
}

ConfigBlock	&ConfigBlock::operator=(const ConfigBlock &other)
{
	if (&other != this)
	{
		this->_knownDirectives = other._knownDirectives;
		this->_io_directive_parameters =  other._io_directive_parameters;
		this->_directive_parameters =  other._directive_parameters;
	}
	return *this;
}

ConfigBlock::~ConfigBlock(void) {}

bool	ConfigBlock::inDirectives(const std::string &directive_name) const
{
	return (this->_directive_parameters.end() != \
		this->_directive_parameters.find(directive_name));
}

bool	ConfigBlock::inIODirectives(const std::string &directive_name) const
{
	return (this->_io_directive_parameters.end() != \
		this->_io_directive_parameters.find(directive_name));
}

const std::string	&ConfigBlock::getDirectiveParameter(\
	const std::string &directive_name)
{
	_map_para_t::const_iterator	it_end = \
		this->_directive_parameters.end();
	_map_para_t::const_iterator	it = \
		this->_directive_parameters.find(directive_name);
	if (it == it_end)
	{
		this->_directive_parameters[""].push_back(""); // !! creates ""
		return this->_directive_parameters[""].front();
	}
	return it->second.front();
}

const ConfigBlock::parameters_t &ConfigBlock::getDirectiveParameters(\
	const std::string &directive_name)
{
	_map_para_t::const_iterator	it_end = \
		this->_directive_parameters.end();
	_map_para_t::const_iterator	it = \
		this->_directive_parameters.find(directive_name);
	if (it == it_end)
	{
		this->_directive_parameters[""].push_back(""); // !! creates ""
		return this->_directive_parameters[""];
	}
	return (it->second);
}

bool	ConfigBlock::inDirectiveParameters(\
	const std::string &directive_name, \
	const std::string &parameter)
{
	_map_para_t::const_iterator	it_end = \
		this->_directive_parameters.end();
	_map_para_t::const_iterator	it = \
		this->_directive_parameters.find(directive_name);
	if (it == it_end)
		return false;
	parameters_t::const_iterator	p_end = \
		it->second.end();
	return (p_end != std::find(it->second.begin(), p_end, parameter));
}

const std::string	&ConfigBlock::getDirectiveOutput(\
	const std::string &directive_name,\
	const std::string &input)
{
	_io_parameters_t::const_iterator	io_it_end = \
		this->_io_directive_parameters.end();
	_io_parameters_t::const_iterator	io_it = \
		this->_io_directive_parameters.find(directive_name);
	if (io_it == io_it_end)
		return this->_io_directive_parameters[""][""]; // !! creates ""
	std::map<std::string, std::string>::const_iterator	it_end = \
		io_it->second.end();
	std::map<std::string, std::string>::const_iterator	it = \
		io_it->second.find(input);
	if (it_end == it)
		return this->_io_directive_parameters[""][""]; // !! creates ""
	return it->second;
}

//

ConfigBlock::_map_para_t	&ConfigBlock::_getDirectiveParametersRef(void)
{
	return this->_directive_parameters;
}

int	ConfigBlock::_addDirective_unique(\
	const std::string &name,\
	const Directive::args_t &parameters)
{
	if (parameters.empty())
		return CF_ERRNOPARAMS;
	this->_directive_parameters[name] = parameters;
	return 0;
}

int	ConfigBlock::_addDirective_io(\
	const std::string &name,\
	Directive::args_t parameters)
{
	if (parameters.empty())
		return CF_ERRNOPARAMS;
	const std::string output = parameters.back();

	parameters.pop_back();
	if (parameters.empty())
		return CF_ERRNOPARAMS;
	const Directive::args_t::const_iterator	it_end = parameters.end();

	for (Directive::args_t::iterator it = parameters.begin(); \
		it_end != it; \
		++it)
			this->_io_directive_parameters[name][*it] = output;
	return 0;
}

int		ConfigBlock::_addDirective(Directive* directive)
{
	Directive::args_t	args_copy = directive->getArgs();

	if (args_copy.empty())
		return (0);
	std::string			name = args_copy.front();
	if (name.empty())
		return CF_ERRNODIR;
	else if (!this->_knownDirective(name))
		return CF_ERRUNKNDIR;
	args_copy.pop_front();
	if ("cgi" == name || "error_page" == name)
		return this->_addDirective_io(name, args_copy);
	else
		return this->_addDirective_unique(name, args_copy);
	return 0;
}

//

void	ConfigBlock::_debug_print(void)
{
	_map_para_t	&map = this->_directive_parameters;
	_map_para_t::const_iterator directive_end = map.end();
	for (_map_para_t::iterator it = map.begin(); it != directive_end; ++it)
	{
		std::cout << "Directive:" << it->first << " ";
		parameters_t				&parameters = it->second;
		parameters_t::iterator	parameters_end = parameters.end();
		for (parameters_t::iterator p_it = parameters.begin(); p_it != parameters_end; ++p_it)
			std::cout << *p_it << " ";
		std::cout << std::endl;
	}
}

bool	ConfigBlock::_knownDirective(const std::string &directive_name)
{
	if (0 == this->_knownDirectives)
		return false;
	parameters_t			&knownDirectives = *this->_knownDirectives;
	parameters_t::iterator	it_end = knownDirectives.end();

	return it_end != std::find(knownDirectives.begin(), it_end, directive_name);
}

void	ConfigBlock::setKnownDirectives(parameters_t *knownDirectives)
{
	this->_knownDirectives = knownDirectives;
}
