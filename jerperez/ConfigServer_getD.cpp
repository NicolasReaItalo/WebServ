/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer_getD.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/01 12:55:17 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBlock.hpp"
#include "ConfigServer.hpp"
#include <string>

/* Returns first directive parameter from directive_name at location
 * location = server.getLocation(uri)
 * valid directive_name can be checked using inIODirectives(location, directive_name)
 * Ex: `root /var/www;' -> server.getDirectiveParameter(-1, "root") -> "/var/www"
 * Ex: `potato /var/www;' -> server.getDirectiveParameter(-1, "root") -> ""
 * Ex: `potato foo bar' -> server.getDirectiveParameter(-1, "potato") -> "foo"
 */
const std::string &ConfigServer::getDirectiveParameter(\
	int location, \
	std::string directive_name) const
{
	if (-1 != location)
	{
		const ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return lConfig.getDirectiveParameter(directive_name);
	}
	return ConfigBlock::getDirectiveParameter(directive_name);
}

/* Returns directive output at location
 * location = server.getLocation(uri)
 * valid directive_name can be checked using inIODirectives(location, directive_name)
 * Returns "" if directive_name is invalid or input is invalid
 * Ex: `cgi .py /python3; cgi .foo /bar;' 
 * 		-> server.getDirectiveOutput(-1, "cgi", ".py") -> "/python3"
 * 		-> server.getDirectiveOutput(-1, "cgi", ".foo") -> "/bar"
 * 		-> server.getDirectiveOutput(-1, ".foo", "/bar") -> ""
 */
const std::string	&ConfigServer::getDirectiveOutput(\
	int location, \
	std::string directive_name, \
	std::string input) const
{
	if (-1 != location)
	{
		const ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inIODirectives(directive_name)) // 
			return lConfig.getDirectiveOutput(directive_name, input);
	}
	return ConfigBlock::getDirectiveOutput(directive_name, input);
}

/* Returns a container with the parameters at location
 * location = server.getLocation(uri)
 * valid directive_name can be checked using inDirectives(location, directive_name)
 * Ex: `limit POST GET;' 
 * 		-> server.getDirectiveParameters(-1, "limit") -> {"POST", "GET"}
 * 		-> server.getDirectiveParameters(-1, "potato") -> {""}
 */
const ConfigServer::parameters_t	&ConfigServer::getDirectiveParameters(int location, std::string directive_name) const
{
	if (-1 != location)
	{
		const ConfigLocation	&lConfig = this->_locations[location];
		if (lConfig.inDirectives(directive_name))
			return lConfig.getDirectiveParameters(directive_name);
	}
	return ConfigBlock::getDirectiveParameters(directive_name);
}
