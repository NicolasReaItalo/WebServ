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

#include <algorithm>
#include "ConfigBlock.hpp"

bool	ConfigBlock::_knownDirective(const std::string &directive_name) const
{
	if (0 == this->_knownDirectives)
		return false;
	const parameters_t					&knownDirectives = \
		*this->_knownDirectives;
	const parameters_t::const_iterator	it_end = \
		knownDirectives.end();

	return it_end != std::find(knownDirectives.begin(), it_end, directive_name);
}

void	ConfigBlock::setKnownDirectives(parameters_t *knownDirectives)
{
	this->_knownDirectives = knownDirectives;
}
