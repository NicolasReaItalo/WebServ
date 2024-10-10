/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/10 10:20:02 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	PARSER_HPP
# define PARSER_HPP

# include "DirectiveBlock.hpp"
# include "Directive.hpp"
# include "tokenizer.hpp"
# include "ConfigServer.hpp"
# include <list>

int	pr_next_directive(
	token_deq_t::iterator &it_list,
	const token_deq_t::const_iterator &it_end,
	DirectiveBlock *context,
	DirectiveBlock &next_directive);

int	pr_parse(token_deq_t &list, std::list<ConfigServer> &servers);

#endif