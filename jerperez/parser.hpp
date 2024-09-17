/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/11 13:36:18 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	PARSER_HPP
# define PARSER_HPP

# include "Directive.hpp"
# include "tokenizer.hpp"

Directive	*pr_next_directive(
	token_deq_t::iterator &it_list,
	token_deq_t::const_iterator it_end, 
	Directive *context);

int	pr_parse_config(token_deq_t &list);

#endif