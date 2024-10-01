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
#include <algorithm>
#include "ConfigBlock.hpp"

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

bool	ConfigBlock::inDirectiveParameters(\
	const std::string &directive_name, \
	const std::string &parameter) const
{
	const _map_para_t::const_iterator	it_end = \
		this->_directive_parameters.end();
	const _map_para_t::const_iterator	it = \
		this->_directive_parameters.find(directive_name);

	if (it == it_end)
		return false;
	const parameters_t::const_iterator	p_end = it->second.end();

	return (p_end != std::find(it->second.begin(), p_end, parameter));
}
