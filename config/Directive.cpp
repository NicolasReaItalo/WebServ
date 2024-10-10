/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/19 16:11:31 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Directive.hpp"

Directive::Directive(void) : _type(-1) {};

Directive::~Directive(void) {}

const int	&Directive::getType(void) const
{
	return this->_type;
}

void		Directive::setType(const int type)
{
	this->_type = type;
}

const std::deque<std::string>	&Directive::getArgs(void) const
{
	return this->_args;
}

std::deque<std::string>	&Directive::getArgsRef(void)
{
	return this->_args;
}

Directive*	Directive::getContext(void) const
{
	return this->_context;
}

void	Directive::setContext(Directive* context)
{
	this->_context = context;
}
