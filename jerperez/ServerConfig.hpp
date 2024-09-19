/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/17 10:26:33 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include <string>
#include <list>
#include <map>
#include <vector>
#include "BlockDirective.hpp"
#include "Directive.hpp"
#include "BlockSimpleConfig.hpp"
#include "LocationConfig.hpp"

class ServerConfig : public BlockSimpleConfig
{
	private:
		std::string					_address;
		std::string					_port;
		std::list<std::string>		_server_names;
		std::vector<LocationConfig>	_locations;
		// directive_parameters_t	&_strToParameters(std::string);
		// directive_parameters_t	&_strToParameters(std::string, char);
		void			_pushSplitParameters(std::string name, std::string joined_string, char delimiter);
		int				_evalListen(void);
		int				_addLocation(BlockDirective* block_directive);
		LocationConfig	&_returnLocation(const std::string &uri);
		void			_debug_print(void);
		int				_fillAll(void);
		
	public:
		ServerConfig(void) {};
		~ServerConfig(void) {};
		ServerConfig(const ServerConfig &other);
		ServerConfig &operator=(const ServerConfig &other);

		std::string	getFullPath(const std::string &decoded_uri, int location);
		int			getLocation(const std::string &decoded_uri);
		std::string	getDirectiveOutput(int location, std::string directive_name, std::string input);
		std::string	getCustomErrorPage(int location, int errorCode); //
		bool		serverCmp(std::string address, std::string port, std::string server_name="");

	 	bool		inDirectives(int location, std::string directive_name);
		std::string getDirectiveParameter(int location, std::string directive_name);
		bool		inDirectiveParameters(int location, std::string directive_name, std::string parameter);
		directive_parameters_t	getDirectiveParameters(int location, std::string directive_name);
		int			addServer(Directive* unknown_directive);
		const std::string				&getAddress(void) const;
		const std::string				&getPort(void) const;
		const std::list<std::string>	&getServerNames(void) const;
	// 	std::string get_error_page(std::string request_uri, int error_code);
	// 	bool get_autoindex_status(std::string request_uri);
	// 	bool get_method_status(std::string request_uri, std::string method); // est-ce-que la methode est autorisee?
	// 	bool get_cgi_status(std::string request_uri, std::string cgi); // est-ce-que le cgi est autorisee?
	// 	int get_return(std::string request_uri, std::string &new_host); // renvoie -1 pas de redirection | redirection : 301 ou 302
	// 																  // et set la reference new_host
	// 	std::string get_directive( std::string request_uri, std::string directive);
	// 		// std::string get_full_ressource_path(std::string uri);
	// 		// std::string get_client_max_body_size(std::string uri);
};

#endif