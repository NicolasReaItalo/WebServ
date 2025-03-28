/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBlock.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/17 13:56:53 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CONFIGBLOCK_HPP
# define CONFIGBLOCK_HPP

# include <string>
# include <map>
# include "Directive.hpp"

class ConfigBlock
{
	public:
		/*Equivalent to std::deque<std::string>*/
		typedef Directive::args_t	parameters_t;
	protected:
		typedef std::map<std::string, parameters_t> \
									_map_para_t;
		typedef std::map<std::string, std::string> \
									_io_t;
		typedef std::map<std::string, _io_t > \
									_io_parameters_t;
		//
		parameters_t				*_knownDirectives;
		_io_parameters_t			_io_directive_parameters;
		_map_para_t					_directive_parameters;
		const std::string			_no_parameter;
		const parameters_t			_no_parameters;
		//
		int						_addDirective_unique(\
									const std::string &name,\
									const Directive::args_t &parameters);
		int						_addDirective_io(\
									const std::string &name,\
									Directive::args_t parameters);
		bool					_knownDirective(\
									const std::string &directive_name) const;
	protected:
		ConfigBlock(void);
	public:
		ConfigBlock(const ConfigBlock &other);
		ConfigBlock &operator=(const ConfigBlock &other);
		~ConfigBlock(void);
		//
		void				_debug_print(void);
		int					_addDirective(Directive*);
		_map_para_t			&_getDirectiveParametersRef(void);
		//
		bool				inDirectives(\
								const std::string &directive_name) const;
		bool				inIODirectives(\
								const std::string &directive_name) const;
		const std::string 	&getDirectiveParameter(\
								const std::string &directive_name) const;
		const parameters_t	&getDirectiveParameters(\
								const std::string &directive_name) const;
		const std::string 	&getDirectiveOutput(\
								const std::string &directive_name,\
								const std::string &input) const;
		bool				inDirectiveParameters(\
								const std::string &directive_name,\
								const std::string &parameter) const;
		//
		void				setKnownDirectives(\
								parameters_t *knownDirectives);
};

#endif
