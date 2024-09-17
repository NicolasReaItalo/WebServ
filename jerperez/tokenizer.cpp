/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 15:21:14 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/09 17:12:55 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unquote.hpp"
#include "tokenizer.hpp"
#include <string> 
#include <cstring>

static void	_add_c_to_token(unsigned char c, int *quote_char, std::string &word, token_deq_t &list)
{
	char	*p_c;

	if (0 == *quote_char)
	{
		p_c = (char *)(std::strchr(TK_METACHAR, c));
		if (NULL != p_c)
		{
			t_token	tk;
			if (false == word.empty())
			{
				if (TK_COMMENT == word[0])
				{
					word.erase();
					*quote_char = TK_COMMENT;
					return ;
				}
				tk.word = word;
				tk_unquote(tk.word); // Should not be there if variable expansion
				tk.token_id = -1;
				list.push_back(tk);	
				word.erase();
			}
			tk.word = "";
			tk.token_id = *p_c;
			list.push_back(tk);
		}
		else
			word += c;
	}
	else
		word += c;
}

static void	_add_c_no_quote(unsigned char c, int *quote_char, std::string &word, token_deq_t &list)
{
		if (TK_QUOTE == c)
			*quote_char = TK_QUOTE;
		else if (TK_ESCAPE == c)
			*quote_char = TK_ESCAPE;
		_add_c_to_token(c, quote_char, word, list);
}

static void	_add_c_double_quote(unsigned char c, int *quote_char, std::string &word, token_deq_t &list)
{
	if (TK_QUOTE == c)
		*quote_char = 0;
	_add_c_to_token(c, quote_char, word, list);
}

static void	_add_c_escape_quote(unsigned char c, int *quote_char, std::string &word, token_deq_t &list)
{
	_add_c_to_token(c, quote_char, word, list);
	*quote_char = 0;
}

/* Tokenize using [space]\{} from line
 * Quoting using double quote "", escape character \, comment #
 */
static int	_add_token_quote(std::string line, token_deq_t &list, std::string &word, int *quote_char)
{
	const int		length = line.length();
	unsigned char	c;

	for (int i = 0; i < length; ++i)
	{
		c = line[i];
		if (0 == *quote_char)
			_add_c_no_quote(c, quote_char, word, list);
		else if (TK_QUOTE == *quote_char)
			_add_c_double_quote(c, quote_char, word, list);
		else if (TK_ESCAPE == *quote_char)
			_add_c_escape_quote(c, quote_char, word, list);
		else if (TK_COMMENT == *quote_char)
		{
			*quote_char = 0;
			return (0);
		}
		else
			return (0xBADC0DE);
	}
	if (TK_COMMENT == *quote_char)
		*quote_char = 0;
	else if (0 == *quote_char)
		_add_c_to_token(TK_SPACE_NEWLINE, quote_char, word, list);
	else
		word += '\n';
	return (0);
}

/* Tokenizes file
 */
int	tk_tokenize(std::fstream &input_file, token_deq_t &list)
{
	std::string			line;
	int					errcode;
	int					quote_char;
	std::string			word("");

	quote_char = 0;
	errcode = 0;
	while (std::getline(input_file, line) && 0 == errcode)
		errcode = _add_token_quote(line, list, word, &quote_char);
	if (errcode)
		return (errcode);
	else if (quote_char)
		return (1);
	return (0);
}
