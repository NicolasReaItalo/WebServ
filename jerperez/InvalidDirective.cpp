/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InvalidDirective.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/06 13:57:32 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "InvalidDirective.hpp"
#include "Directive.hpp"

InvalidDirective::InvalidDirective(
	std::deque<std::string> args, Directive *context, std::string what) :
		Directive(-1, args, context),
		_what(what)
{}

InvalidDirective::~InvalidDirective(void) {}

const std::string	&InvalidDirective::getWhat(void) const
{
	return(this->_what);
}