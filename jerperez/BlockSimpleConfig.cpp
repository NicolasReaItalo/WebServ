/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlockSimpleConfig.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/17 13:58:10 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <list>
#include <map>
#include <algorithm>
#include "BlockSimpleConfig.hpp"
#include "BlockDirective.hpp"
#include "Directive.hpp"
#include <iostream>
#include <stdexcept>

bool	BlockSimpleConfig::inDirectives(std::string directive_name)
{
	return (this->_directive_parameters.end() != \
		this->_directive_parameters.find(directive_name));
}

bool	BlockSimpleConfig::inIODirectives(std::string directive_name)
{
	return (this->_io_directive_parameters.end() != \
		this->_io_directive_parameters.find(directive_name));
}

std::string	BlockSimpleConfig::getDirectiveParameter(std::string directive_name)
{
	_directive_parameters_map_t::const_iterator	it_end = \
		this->_directive_parameters.end();
	_directive_parameters_map_t::const_iterator	it = \
		this->_directive_parameters.find(directive_name);
	if (it == it_end)
		return ("");
	return (it->second.front());
}

std::string	BlockSimpleConfig::getDirectiveOutput(std::string directive_name, std::string input)
{
	_io_directive_parameters_t::const_iterator	io_it_end = \
		this->_io_directive_parameters.end();
	_io_directive_parameters_t::const_iterator	io_it = \
		this->_io_directive_parameters.find(directive_name);
	if (io_it == io_it_end)
		return ("");
	std::map<std::string, std::string>::const_iterator	it_end = \
		io_it->second.end();
	std::map<std::string, std::string>::const_iterator	it = \
		io_it->second.find(input);
	if (it_end == it)
		return ("");
	return (it->second);
}

BlockSimpleConfig::directive_parameters_t BlockSimpleConfig::getDirectiveParameters(std::string directive_name)
{
	_directive_parameters_map_t::const_iterator	it_end = \
		this->_directive_parameters.end();
	_directive_parameters_map_t::const_iterator	it = \
		this->_directive_parameters.find(directive_name);
	if (it == it_end)
	{
		BlockSimpleConfig::directive_parameters_t	nothing;
		return (nothing);
	}
	return (it->second);
}

bool	BlockSimpleConfig::inDirectiveParameters(\
	std::string directive_name, \
	std::string parameter)
{
	_directive_parameters_map_t::const_iterator	it_end = \
		this->_directive_parameters.end();
	_directive_parameters_map_t::const_iterator	it = \
		this->_directive_parameters.find(directive_name);
	if (it == it_end)
		return (false);
	directive_parameters_t::const_iterator	p_end = \
		it->second.end();
	return (p_end != std::find(it->second.begin(), p_end, parameter));
}

int	BlockSimpleConfig::_addDirective_unique(std::string &name, Directive::args_t::iterator &it, Directive::args_t::const_iterator &it_end)
{
	directive_parameters_t				parameters;

	while (it != it_end)
		parameters.push_back(*it++);
	this->_directive_parameters[name] = parameters;
	return (0);
}

int	BlockSimpleConfig::_addDirective_io(std::string &name, Directive::args_t::iterator &it, Directive::args_t::const_iterator &it_end)
{
	Directive::args_t::const_iterator	it_io_end = it_end - 1;

	if (it_io_end == it)
		return (1);
	while (it != it_io_end)
		this->_io_directive_parameters[name][*it++] = *it_io_end;
	(void)name;
	return (0);
}

int		BlockSimpleConfig::_addDirective(Directive* directive)
{
	Directive::args_t	args = directive->getArgs();

	if (args.empty())
		return (0);
	Directive::args_t::iterator			it = args.begin();
	Directive::args_t::const_iterator	it_end = args.end();
	std::string							name = *it;
	if (name.empty())
		return (1);
	else if (!this->knownDirective(name))
	{
		std::cerr << "BlockSimpleConfig: unknown directive `" << name << "'" << std::endl; //
		return (1);
	}
	++it;
	if ("cgi" == name || "error_page" == name)
		return (this->_addDirective_io(name, it, it_end));
	else
		return (this->_addDirective_unique(name, it, it_end));
	return (0);
}

//DEBUG REMOVE ME

void	BlockSimpleConfig::_debug_print(void)
{
	_directive_parameters_map_t	&map = this->_directive_parameters;
	_directive_parameters_map_t::const_iterator directive_end = map.end();
	for (_directive_parameters_map_t::iterator it = map.begin(); it != directive_end; ++it)
	{
		std::cout << "Directive:" << it->first << " ";
		directive_parameters_t				&parameters = it->second;
		directive_parameters_t::iterator	parameters_end = parameters.end();
		for (directive_parameters_t::iterator p_it = parameters.begin(); p_it != parameters_end; ++p_it)
			std::cout << *p_it << " ";
		std::cout << std::endl;
	}
}

bool	BlockSimpleConfig::knownDirective(std::string directive_name)
{
	if (0 == this->_knownDirectives)
		return (false);
	directive_parameters_t					&knownDirectives = *this->_knownDirectives;
	directive_parameters_t::iterator		it_end = knownDirectives.end();

	return (it_end != std::find(knownDirectives.begin(), it_end, directive_name));
}

void	BlockSimpleConfig::setKnownDirectives(std::list<std::string> *knownDirectives)
{
	this->_knownDirectives = knownDirectives;
}

int	BlockSimpleConfig::_updateErrorPages(directive_parameters_t &error_page_params)
{
	directive_parameters_t::iterator	it_end = error_page_params.end();
	directive_parameters_t::iterator	it = error_page_params.begin();

	if (it_end == error_page_params.begin())
		return (1);
	std::string	uri = error_page_params.back();
	directive_parameters_t::iterator	it_err_end = --it_end;

	if (it_err_end == it)
		return (1);
	int	err_code;

	while (it != it_err_end)
	{
		err_code = std::atoi(it->c_str());
		this->_error_pages[err_code] = uri;
		++it;
	}
	return (0);
}

std::string	BlockSimpleConfig::getCustomErrorPage(int errorCode)
{
	std::map<int, std::string>	&error_pages = this->_error_pages;
	std::map<int, std::string>::iterator	it = error_pages.find(errorCode);

	if (error_pages.end() == it)
		return ("");
	return (it->second);
}
