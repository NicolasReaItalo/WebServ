/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/06 16:25:44 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Directive.hpp"

Directive::Directive(int type, std::deque<std::string> args, Directive *context) :
	_type(type),
	_args(args),
	_context(context)
{}

Directive::Directive(int type) :
	_type(type)
{}

Directive::Directive(std::deque<std::string> args, Directive *context) :
	_type(PR_DIR_TYPE_SIMPLE),
	_args(args),
	_context(context)
{}

Directive::Directive(int type, std::deque<std::string> args) :
	_type(type),
	_args(args)
{}

Directive::~Directive(void) {}

const int	&Directive::getType(void) const
{
	return (this->_type);
}

const std::deque<std::string>	&Directive::getArgs(void) const
{
	return (this->_args);
}


Directive*	Directive::getContext(void) const
{
	return (this->_context);
}

void	Directive::setContext(Directive* context)
{
	this->_context = context;
}

int	Directive::evaluate(funmap_t &map)
{
	funmap_arg_t	&args = this->_args;

	if (true == args.empty())
		return (0);

	funmap_t::iterator	it = map.find(args.front());

	if (it == map.end())
		return (127);
	return (it->second(args, this->_context));
}
