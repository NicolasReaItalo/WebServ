/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlockDirective.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/06 15:16:43 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "BlockDirective.hpp"

static std::deque<std::string>	_arg_main(void)
{
	std::deque<std::string> args;

	args.push_back("main");
	return (args);
}

BlockDirective::BlockDirective(void) :
	Directive(PR_DIR_TYPE_BLOCK, _arg_main()),
	_block_opened(false)
{}

// BlockDirective::BlockDirective(int type, std::deque<std::string> args, std::deque<Directive*> instructions, Directive *context) :
// 	Directive(type, args, context),
// 	_instructions(instructions)
// {}

// BlockDirective::BlockDirective(int type, std::deque<std::string> args, Directive *context) :
// 	Directive(type, args, context)
// {}

BlockDirective::BlockDirective(std::deque<std::string> args, std::deque<Directive*> instructions, Directive *context) :
	Directive(PR_DIR_TYPE_BLOCK, args, context),
	_instructions(instructions),
	_block_opened(false)
{}

BlockDirective::BlockDirective(std::deque<std::string> args, Directive *context) :
	Directive(PR_DIR_TYPE_BLOCK, args, context),
	_block_opened(false)
{}

BlockDirective::~BlockDirective(void)
{
	std::deque<Directive*>::iterator		it_end = this->_instructions.end();

	for (std::deque<Directive*>::iterator it = this->_instructions.begin(); it != it_end; ++it)
		delete (*it);
}

const std::deque<Directive*>	&BlockDirective::getInstructions(void) const
{
	return (this->_instructions);
}

void	BlockDirective::addInstruction(Directive *d)
{
	if (0 != d)
		this->_instructions.push_back(d);
}

bool	BlockDirective::openBlock(void)
{
	if (true == this->_block_opened)
		return (false);
	this->_block_opened = true;
	return (true);
}

bool	BlockDirective::closeBlock(void)
{
	if (false == this->_block_opened)
		return (false);
	this->_block_opened = false;
	return (true);
}