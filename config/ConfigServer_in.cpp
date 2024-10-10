/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer_in.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/03 11:15:20 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBlock.hpp"
#include "ConfigServer.hpp"
#include <string>

/* Returns true if directive_name in Directives at location
 * Returns false else
 * Undefined for directive_name = ""
 * location = server.getLocation(uri)
 * Ex: `root /var/www;' -> server.inDirectives(-1, "root") -> true
 * Ex: `potato /var/www;' -> server.inDirectives(-1, "root") -> false
 */
bool		ConfigServer::inDirectives(\
	int location, \
	std::string directive_name) const
{
	if (-1 == location)
		return ConfigBlock::inDirectives(directive_name);
	if (0 > location || this->_locations.size() <= (unsigned int)location)
		return false;
	return this->_locations[location].inDirectives(directive_name);
}

/* Returns true if directive_name in Directives at location
 * Returns false else
 * Undefined for directive_name = ""
 * location = server.getLocation(uri)
 * Ex: `error_page 404 /error.html;' -> server.inDirectives(-1, "error_page") -> true
 * Ex: `error_page 404 /error.html;' -> server.inDirectives(-1, "potato") -> false
 */
bool	ConfigServer::inIODirectives(\
	int location, \
	std::string directive_name) const
{
	if (-1 == location)
		return ConfigBlock::inIODirectives(directive_name);
	if (0 > location || this->_locations.size() <= (unsigned int)location)
		return false;
	return this->_locations[location].inIODirectives(directive_name);
}

/* Returns true if the parameter is among the parameters at location
 * location = server.getLocation(uri)
 * valid directive_name can be checked using inDirectives
 * Returns false if directive_name is invalid or parameter is not found
 * Undefined for directive_name = ""
 * Ex: `limit POST GET;' 
 * 		-> server.inDirectiveParameters(-1, "limit", "GET") -> true
 * 		-> server.inDirectiveParameters(-1, "potato", "POST") -> false
 * 		-> server.inDirectiveParameters(-1, "limit", "limit") -> false
 */
bool	ConfigServer::inDirectiveParameters(\
	int location, \
	std::string directive_name, \
	std::string parameter) const
{
	if (-1 != location)
	{
		const ConfigLocation	&lConfig = this->_locations[location];
		if (0 > location || this->_locations.size() <= (unsigned int)location)
			return false;
		if (lConfig.inDirectives(directive_name))
			return lConfig.inDirectiveParameters(directive_name, parameter);
	}
	return ConfigBlock::inDirectiveParameters(directive_name, parameter);
}
