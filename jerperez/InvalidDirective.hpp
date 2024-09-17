/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InvalidDirective.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/06 13:56:59 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	INVALIDDIRECTIVE_HPP
# define INVALIDDIRECTIVE_HPP

# include "Directive.hpp"
# include <deque>
# include <string>

class InvalidDirective : public Directive
{
	private:
		InvalidDirective(void);
		InvalidDirective(std::deque<std::string>, Directive *);
		std::string	_what;
	public:
		InvalidDirective(std::deque<std::string>, Directive *, std::string);
		~InvalidDirective(void);
		const std::string	&getWhat(void) const;
};

#endif