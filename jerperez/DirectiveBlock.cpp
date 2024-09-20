/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectiveBlock.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/19 16:34:30 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "DirectiveBlock.hpp"

static int	_block_state_none(void) {return 0;}
static int	_block_state_opened(void) {return 1;}
static int	_block_state_closed(void) {return 2;}

DirectiveBlock::DirectiveBlock(void) : _block_state(_block_state_none()) {}

DirectiveBlock::~DirectiveBlock(void) {}

const DirectiveBlock::instructions_t	&DirectiveBlock::getInstructions(void) const
{
	return this->_instructions;
}

void	DirectiveBlock::addInstruction(const DirectiveBlock &d)
{
	this->_instructions.push_back(d);
}

int	DirectiveBlock::openBlock(void)
{
	if (_block_state_none() != this->_block_state)
		return 1;
	this->_block_state = _block_state_opened();
	return 0;
}

int	DirectiveBlock::closeBlock(void)
{
	if (_block_state_opened() != this->_block_state)
		return 1;
	this->_block_state = _block_state_closed();
	return 0;
}

DirectiveBlock	&DirectiveBlock::nextInstructionRef(void)
{
	DirectiveBlock	tmp;

	this->_instructions.push_back(tmp);
	return this->_instructions.back();
}
