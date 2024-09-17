/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 15:20:49 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/09 15:39:32 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	TOKENIZER_HPP
# define TOKENIZER_HPP

#include <string>
#include <deque>
#include <iostream>
#include <fstream>

#define TK_SPACE_NEWLINE	'\n'
#define	TK_SPACE			" \t\n"
#define TK_QUOTE			'\"'
#define TK_ESCAPE			'\\'
#define TK_COMMENT			'#'
#define	TK_DIRECTIVE_END	';'
#define	TK_BLOCK_OPEN		'{'
#define	TK_BLOCK_CLOSE		'}'
#define	TK_METACHAR			TK_SPACE";{}"

#define	TK_CHAR

typedef struct s_token
{
	int			token_id;
	std::string	word;
} t_token;

typedef	std::deque<t_token>	token_deq_t;

int	tk_tokenize(std::fstream &input_file, token_deq_t &list);

#endif
