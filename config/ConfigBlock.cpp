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

#include "ConfigBlock.hpp"

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
