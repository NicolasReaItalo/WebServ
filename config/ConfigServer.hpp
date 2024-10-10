/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/01 13:34:40 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include <string>
# include <list>
# include <map>
# include <vector>
# include "DirectiveBlock.hpp"
# include "Directive.hpp"
# include "ConfigBlock.hpp"
# include "ConfigLocation.hpp"

class ConfigServer : public ConfigBlock
{
	private:
		std::string					_address;
		std::string					_port;
		parameters_t				_server_names;
		std::vector<ConfigLocation>	_locations;
		void			_pushSplitParameters(\
							std::string name,\
							std::string joined_string,\
							char delimiter);
		//
		int				_evalListen(void);
		int				_evalServerName(void);
		//
		int				_addLocation(DirectiveBlock* block_directive);
		int				_fillAll(void);
		_map_para_t		&_getDirectiveParametersRef(void);
		//
	protected:
		using				ConfigBlock::_addDirective;
	public:
		ConfigServer(void);
		~ConfigServer(void);
		ConfigServer(const ConfigServer &other);
		ConfigServer &operator=(const ConfigServer &other);
		//
		int			addServer(Directive* unknown_directive);
		//
		std::string			getFullPath(const std::string &decoded_uri, int location) const;
		int					getLocation(const std::string &decoded_uri) const;
		bool				serverCmp(const std::string &address, const std::string &port) const;
		bool				serverCmp(const std::string &address, const std::string &port, const std::string &server_name) const;
	 	bool				inDirectives(int location, std::string directive_name) const;
		bool				inIODirectives(int location, std::string directive_name) const;
		bool				inDirectiveParameters(\
			int location, \
			std::string directive_name, \
			std::string parameter) const;
		const std::string 	&getDirectiveParameter(int location, std::string directive_name) const;
		const parameters_t	&getDirectiveParameters(int location, std::string directive_name) const;
		const std::string	&getDirectiveOutput(int location, std::string directive_name, std::string input) const;
		//
		const std::string	&getAddress(void) const;
		const std::string	&getPort(void) const;
		const parameters_t	&getServerNames(void) const;
		//
		int					_debugPlaceholder(int debug_input=0); //
		void				_debug_print(void); //
};

#endif
