/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer_serverCmp.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/01 12:50:10 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigServer.hpp"
#include <string>
#include <algorithm>

/* Return true is the server matches the address and port
 * Ex: `listen 127.0.0.1:80;' 
 * 		-> server.serverCmp("127.0.0.1", "80") -> true
 * 		-> server.serverCmp("127.0.0.1", "81") -> false
 */
bool	ConfigServer::serverCmp(\
	const std::string &address,\
	const std::string &port) const
{
	return (address == this->_address && port == this->_port);
}

/* Return true is the server matches the address and port
 * and server_name if any
 * Ex: `listen 127.0.0.1:80; server_names foo;' 
 * 		-> server.serverCmp("127.0.0.1", "80", "") -> true
 * 		-> server.serverCmp("127.0.0.1", "80", "foo") -> true
 * 		-> server.serverCmp("127.0.0.1", "80", "bar") -> false
 */
bool	ConfigServer::serverCmp(\
	const std::string &address,\
	const std::string &port,\
	const std::string &server_name) const
{
	if (false == serverCmp(address, port))
		return false;
	if ("" == server_name)
		return true;
	const parameters_t					&server_names = this->_server_names;
	const parameters_t::const_iterator	&it_end = server_names.end();

	return (it_end != std::find(server_names.begin(), it_end, server_name));
}
