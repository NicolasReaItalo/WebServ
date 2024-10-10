/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 15:20:49 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/10 10:05:57 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	TOKENIZER_HPP
# define TOKENIZER_HPP

#include <string>
#include <deque>
#include <fstream>

#define CF_SPACE_NEWLINE	'\n'
#define	CF_SPACE			" \t\n"
#define CF_QUOTE			'\"'
#define CF_ESCAPE			'\\'
#define CF_COMMENT			'#'
#define	CF_DIRECTIVE_END	';'
#define	CF_BLOCK_OPEN		'{'
#define	CF_BLOCK_CLOSE		'}'
#define	CF_METACHAR			CF_SPACE";{}"
#define	CF_ERRQUOTE			127

typedef struct s_token
{
	int			token_id;
	std::string	word;
} t_token;

typedef	std::deque<t_token>	token_deq_t;

int	cf_tokenize(std::fstream &input_file, token_deq_t &list);

#endif
