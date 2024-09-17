/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlockSimpleConfig.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/17 10:28:22 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	BLOCKSIMPLECONFIG_HPP
# define BLOCKSIMPLECONFIG_HPP

#include <string>
#include <list>
#include <map>
#include "Directive.hpp"

class BlockSimpleConfig
{
	public:
		typedef std::list<std::string> \
										directive_parameters_t;
		typedef std::map<std::string, directive_parameters_t> \
										_directive_parameters_map_t;
		typedef std::map<std::string, std::map<std::string, std::string> > \
										_io_directive_parameters_t;
		_directive_parameters_map_t		_directive_parameters;
		std::list<std::string>			*_knownDirectives;
		std::map<int, std::string>		_error_pages;
		_io_directive_parameters_t		_io_directive_parameters;
		void		_debug_print(void);
		int			_updateErrorPages(directive_parameters_t &error_page_params);
		int						_addDirective(Directive*);
	public:
		BlockSimpleConfig(void) : _knownDirectives(0) {};
		bool					knownDirective(std::string directive_name);
		bool					inDirectives(std::string directive_name);
		bool					inIODirectives(std::string directive_name);
		std::string 			getDirectiveParameter(std::string directive_name);
		directive_parameters_t	getDirectiveParameters(std::string directive_name);
		std::string 			getDirectiveOutput(std::string directive_name, std::string input);
		bool					inDirectiveParameters(std::string directive_name, std::string parameter);
		void					setKnownDirectives(std::list<std::string> *knownDirectives);
		std::string				getCustomErrorPage(int errorCode);
};

// class ServerConfig : public BlockSimpleConfig
// {
// 	private:
// 		std::string										_address;
// 		std::string										_port;
// 		std::list<std::string>							_server_names;
// 		std::map<std::string, BlockSimpleConfig> 		_locations;
// 	// public:
// 	// 	std::string get_error_page(std::string request_uri, int error_code);
// 	// 	bool get_autoindex_status(std::string request_uri);
// 	// 	bool get_method_status(std::string request_uri, std::string method); // est-ce-que la methode est autorisee?
// 	// 	bool get_cgi_status(std::string request_uri, std::string cgi); // est-ce-que le cgi est autorisee?
// 	// 	int get_return(std::string request_uri, std::string &new_host); // renvoie -1 pas de redirection | redirection : 301 ou 302
// 	// 																  // et set la reference new_host
// 	// 	std::string get_directive( std::string request_uri, std::string directive);
// 	// 		// std::string get_full_ressource_path(std::string uri);
// 	// 		// std::string get_client_max_body_size(std::string uri);
// };

#endif