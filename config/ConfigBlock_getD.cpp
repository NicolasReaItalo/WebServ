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
#include "ConfigBlock.hpp"

const std::string	&ConfigBlock::getDirectiveParameter(\
	const std::string &directive_name) const
{
	const _map_para_t::const_iterator	it_end = \
		this->_directive_parameters.end();
	const _map_para_t::const_iterator	it = \
		this->_directive_parameters.find(directive_name);

	if (it == it_end)
		return this->_no_parameter;
	return it->second.front();
}

const ConfigBlock::parameters_t &ConfigBlock::getDirectiveParameters(\
	const std::string &directive_name) const
{
	const _map_para_t::const_iterator	it_end = \
		this->_directive_parameters.end();
	const _map_para_t::const_iterator	it = \
		this->_directive_parameters.find(directive_name);

	if (it == it_end)
		return this->_no_parameters;
	return (it->second);
}

const std::string	&ConfigBlock::getDirectiveOutput(\
	const std::string &directive_name,\
	const std::string &input) const
{
	const _io_parameters_t::const_iterator	io_it_end = \
		this->_io_directive_parameters.end();
	const _io_parameters_t::const_iterator	io_it = \
		this->_io_directive_parameters.find(directive_name);

	if (io_it == io_it_end)
		return this->_no_parameter;
	const _io_t::const_iterator	it_end = io_it->second.end();
	const _io_t::const_iterator	it = io_it->second.find(input);

	if (it_end == it)
		return this->_no_parameter;
	return it->second;
}

ConfigBlock::_map_para_t	&ConfigBlock::_getDirectiveParametersRef(void)
{
	return this->_directive_parameters;
}
