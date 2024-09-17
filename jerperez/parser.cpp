/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:54:04 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/09 14:56:04 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "InvalidDirective.hpp"
#include "BlockDirective.hpp"
#include "Directive.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include <string>
#include <cstring>

/* returns true if space
 */
static bool	_isspace(char c)
{
	return (NULL != (char *)(std::strchr(TK_SPACE, c)));
}

/* Gets arguments (words) before token
 * Returns token value, ';' if Simple Directive
 * sets iterator at last valid token (; if simple directive)
 */
static char	_get_directive_args(
	token_deq_t::iterator &it_list,
	token_deq_t::const_iterator it_end,
	std::deque<std::string> &args)
{
	while (it_list != it_end && _isspace(it_list->token_id))
		++it_list;
	while (it_list != it_end && TK_BLOCK_OPEN != it_list->token_id)
	{
		if (-1 == it_list->token_id)
			args.push_back(it_list->word);
		else if (TK_DIRECTIVE_END == it_list->token_id)
			return (TK_DIRECTIVE_END);
		else if (TK_BLOCK_CLOSE == it_list->token_id)
			return (TK_BLOCK_CLOSE);
		if (it_list != it_end)
			++it_list;
		while (it_list != it_end && _isspace(it_list->token_id))
			++it_list;
	}
	return ('\0');
}

/* Gets instructions (Directives) before token
 * Returns token value, '}' if Block Directive
 * sets iterator at last valid token (} if block directive)
 */
static char	_add_block_instructions(
	token_deq_t::iterator &it_list,
	token_deq_t::const_iterator it_end,
	BlockDirective *curr)
{
	Directive	*d;

	while (it_list != it_end)
	{
		if ('{' == it_list->token_id)
			return ('{');
		else if ('}' == it_list->token_id)
			return ('}');
		else
		{
			d = pr_next_directive(it_list, it_end, curr);
			if (d && -1 == d->getType())
				return ('\0');
			curr->addInstruction(d);
		}
		while (it_list != it_end && _isspace(it_list->token_id))
			++it_list;
	}
	return ('\0');
}

/* Gets next directive starting at iterator
 * Sets iterator after last used token
 * Returns new Directive (Directive/BlockDirective)
 * Returns 0 if end
 */
Directive	*pr_next_directive(
	token_deq_t::iterator &it_list,
	token_deq_t::const_iterator it_end,
	Directive *context)
{
	std::deque<std::string>	args;
	char					curr_token;

	if (0 == context)
		return (new InvalidDirective(args, context, "null context"));
	//
	curr_token = _get_directive_args(it_list, it_end, args);
	if (';' == curr_token)
	{
		++it_list;
		return (new Directive(args, context));
	}
	if (true == args.empty())
	{
		if (it_list == it_end)
			return (0);
		else if ('}' == curr_token)
		{
			if (true == reinterpret_cast<BlockDirective*>(context)->closeBlock())
				return (0);
			else
				return (new InvalidDirective(args, context, "unexpected } token, no arguments"));
		}
	}
	else if ((it_list == it_end) || ('}' == curr_token))
		return (new InvalidDirective(args, context, "bad directive"));
	else if ('{' == it_list->token_id)
		++it_list;
	else if (false == args.empty())
		return (new InvalidDirective(args, context, "missing expected ; token"));
	//
	BlockDirective			*new_block;

	if (true == args.empty())
		return (new InvalidDirective(args, context, "block directive missing name"));
	new_block = new BlockDirective(args, context);
	new_block->openBlock();
	curr_token = _add_block_instructions(it_list, it_end, new_block);
	if ('}' == curr_token)
	{
		++it_list;
		return (new_block);
	}
	if (false == new_block->getInstructions().empty())
	{
		delete new_block;
		return (new InvalidDirective(args, context, "missing expected } token"));
	}
	delete new_block;
	return (new InvalidDirective(args, context, "missing expected } token, no instructions"));
}
