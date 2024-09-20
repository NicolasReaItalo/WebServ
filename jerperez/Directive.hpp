/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/19 16:33:50 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	DIRECTIVE_HPP
# define DIRECTIVE_HPP

# include <string>
# include <deque>
# define PR_DIR_TYPE_SIMPLE 1
# define PR_DIR_TYPE_BLOCK 2

class Directive
{
	public:
		typedef std::deque<std::string>	args_t;
	protected:
		int								_type;
		args_t							_args;
		Directive*						_context;
		Directive(void);
	public:
		virtual ~Directive(void);
		const int						&getType(void) const;
		const std::deque<std::string>	&getArgs(void) const;
		Directive*						getContext(void) const;
		//
		void							setType(const int type);
		void							setContext(Directive*);
		//
		std::deque<std::string>			&getArgsRef(void);
};

#endif