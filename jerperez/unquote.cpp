/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unquote.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 15:21:14 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/09 16:02:28 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokenizer.hpp"
#include <string> 

static void	_add_c_no_quote(unsigned char c, int *quote_char, std::string &word)
{
		if (TK_QUOTE == c)
			*quote_char = TK_QUOTE;
		else if (TK_ESCAPE == c)
			*quote_char = TK_ESCAPE;
		else
			word += c;
}

static void	_add_c_double_quote(unsigned char c, int *quote_char, std::string &word)
{
	if (TK_QUOTE == c)
		*quote_char = 0;
	else
		word += c;
}

static void	_add_c_escape_quote(unsigned char c, int *quote_char, std::string &word)
{
	word += c;
	*quote_char = 0;
}

/*  Removes "" end \ from string
 */
void	tk_unquote(std::string &word)
{
	const int		length = word.length();
	std::string		new_str("");
	int				quote_char;

	quote_char = 0;
	for (int i = 0; i < length; ++i)
	{
		if (0 == quote_char)
			_add_c_no_quote(word[i], &quote_char, new_str);
		else if (TK_QUOTE == quote_char)
			_add_c_double_quote(word[i], &quote_char, new_str);
		else if (TK_ESCAPE == quote_char)
			_add_c_escape_quote(word[i], &quote_char, new_str);
	}
	word = new_str;
}
