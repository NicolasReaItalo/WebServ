/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:54:04 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/26 10:59:17 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DirectiveBlock.hpp"
#include "Directive.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include <string>
#include <cstring>

/* skip space
 *
 */
static void	_skip_space(token_deq_t::iterator &it_list,
	const token_deq_t::const_iterator &it_end)
{
	while ((it_list != it_end) \
		&& NULL != (char *)(std::strchr(TK_SPACE, it_list->token_id)))
	{
		++it_list;
	}
}

/* Gets arguments (words) before token
 * Returns token value, ';' if Simple Directive
 * sets iterator at last valid token (; if simple directive)
 */
static char	_get_directive_args(
	token_deq_t::iterator &it_list,
	const token_deq_t::const_iterator &it_end,
	std::deque<std::string> &args)
{
	_skip_space(it_list, it_end);
	while (it_list != it_end && TK_BLOCK_OPEN != it_list->token_id)
	{
		if (-1 == it_list->token_id)
			args.push_back(it_list->word);
		else if (TK_DIRECTIVE_END == it_list->token_id)
			return TK_DIRECTIVE_END;
		else if (TK_BLOCK_CLOSE == it_list->token_id)
			return TK_BLOCK_CLOSE;
		if (it_list != it_end)
			++it_list;
		_skip_space(it_list, it_end);
	}
	if (it_list == it_end)
		return '\0';
	return it_list->token_id;
}

/* Gets instructions (Directives) before token
 * Returns token value, '}' if Block Directive
 * sets iterator at last valid token (} if block directive)
 */
static char	_add_block_instructions(
	token_deq_t::iterator &it_list,
	const token_deq_t::const_iterator &it_end,
	DirectiveBlock *context)
{
	int	err_code;

	while (it_list != it_end)
	{
		if (TK_BLOCK_OPEN == it_list->token_id)
			return TK_BLOCK_OPEN;
		else if (TK_BLOCK_CLOSE == it_list->token_id)
			return TK_BLOCK_CLOSE;
		else
		{
			DirectiveBlock &d = context->nextInstructionRef();
			err_code = pr_next_directive(it_list, it_end, context, d);
			if (err_code)
				return (err_code);
		}
		_skip_space(it_list, it_end);
	}
	return ('\0');
}

/* Updates next directive starting at iterator
 * Sets iterator at current token
 * Returns 0 if OK
 * Returns 127 + char code if error
 * Returns 127 + '{' code if not simple directive
 */
static int	_next_directive_simple(
	token_deq_t::iterator &it_list,
	const token_deq_t::const_iterator &it_end,
	DirectiveBlock *context,
	DirectiveBlock &directive)
{
	std::deque<std::string>	&args = directive.getArgsRef();
	char					curr_token;

	curr_token = _get_directive_args(it_list, it_end, args);
	if (TK_DIRECTIVE_END == curr_token)
		return 0;
	if (true == args.empty())
	{
		if (true == (it_list == it_end))
			return 0;
		if (TK_BLOCK_CLOSE == curr_token)
		{
			if (context->closeBlock())
				return PR_ERRDBADTOKEN + curr_token;
			else
				return 0;
		}
	}
	else if (it_list == it_end || TK_BLOCK_CLOSE == curr_token)
		return PR_ERRDNOTCLOSED;
	else if (TK_BLOCK_OPEN == curr_token)
		return PR_ERRDBADTOKEN + TK_BLOCK_OPEN;
	return PR_ERRDSIMPLE;
}

/* Updates next directive starting at iterator
 * Sets iterator at current token
 * Returns 0 if end
 */
static int	_next_directive_block(
	token_deq_t::iterator &it_list,
	const token_deq_t::const_iterator &it_end,
	DirectiveBlock &next_directive)
{
	if (true == next_directive.getArgs().empty())
		return PR_ERRDBNOARGS;
	if (next_directive.openBlock())
		return PR_ERRDOPENFAIL;
	if (TK_BLOCK_CLOSE == _add_block_instructions(it_list, it_end, &next_directive))
		return (0);
	return PR_ERRDNOTCLOSED;
}

/* Gets next directive starting at iterator
 * Sets iterator after at last used token
 * context cannot be NULL
 */
int	pr_next_directive(
	token_deq_t::iterator &it_list,
	const token_deq_t::const_iterator &it_end,
	DirectiveBlock *context,
	DirectiveBlock &next_directive)
{
	int	err_code;

	if (0 == context)
		return PR_ERRDNOCONTEXT;
	next_directive.setContext(context);
	err_code = _next_directive_simple(it_list, it_end, context, next_directive);
	if (0 == err_code)
	{
		next_directive.setType(PR_DIR_TYPE_SIMPLE);
		if (it_list != it_end)
			++it_list;
		_skip_space(it_list, it_end);
		return 0;
	}
	else if ((PR_ERRDBADTOKEN + TK_BLOCK_OPEN) != err_code)
	{
		next_directive.setType(-1);
		return err_code;
	}
	if (it_end != it_list)
		++it_list;
	if (0 == _next_directive_block(it_list, it_end, next_directive))
	{
		next_directive.setType(PR_DIR_TYPE_BLOCK);
		if (it_list != it_end)
			++it_list;
		_skip_space(it_list, it_end);
		return 0;
	}
	next_directive.setType(-1);
	return PR_ERRDBLOCK;
}
