/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBlock.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/19 16:05:55 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBlock.hpp"
#include <iostream>

void	ConfigBlock::_debug_print(void)
{
	const _map_para_t	&map = this->_directive_parameters;
	const _map_para_t::const_iterator directive_end = map.end();

	for (_map_para_t::const_iterator it = map.begin(); it != directive_end; ++it)
	{
		std::cout << "Directive:" << it->first << " ";
		const parameters_t					&parameters = it->second;
		const parameters_t::const_iterator	&parameters_end = parameters.end();

		for (parameters_t::const_iterator p_it = parameters.begin(); \
				p_it != parameters_end; \
				++p_it)
			std::cout << *p_it << " ";
		std::cout << std::endl;
	}
}
