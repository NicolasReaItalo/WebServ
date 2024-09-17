/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 17:31:53 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/10 14:25:22 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	DIRECTIVE_HPP
# define DIRECTIVE_HPP

# include <string>
# include <deque>
# include <map>
# define PR_DIR_TYPE_SIMPLE 1
# define PR_DIR_TYPE_BLOCK 2

class Directive
{
	public:
		typedef std::deque<std::string>	args_t;
	private:
		Directive(void) : _type(PR_DIR_TYPE_SIMPLE) {};
	protected:
		const int						_type;
		args_t							_args;
		Directive*						_context;
		Directive(int type, std::deque<std::string> args, Directive* context);
		Directive(int type, std::deque<std::string> args);
		Directive(int type);
	public:
		Directive(std::deque<std::string> args, Directive* context);
		typedef std::deque<std::string>							funmap_arg_t;
		typedef std::map<std::string, int (*)(funmap_arg_t, Directive*)>	funmap_t;
		virtual ~Directive(void);
		const int						&getType(void) const;
		const std::deque<std::string>	&getArgs(void) const;
		Directive*						getContext(void) const;
		void							setContext(Directive*);
		int								evaluate(funmap_t&);
};

#endif