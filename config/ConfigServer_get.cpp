/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer_get.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/10 10:16:09 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBlock.hpp"
#include "ConfigServer.hpp"
#include <string>
#include <sstream>
//#include <iostream> // DEBUG
#include <limits.h>
#include <stdlib.h>
#include <algorithm>

/* Returns the full path from decoded_uri at location
 * location = server.getLocation(decoded_uri)
 * takes into account the alias ant root directives
 * with alias taking priority over root 
 * Does not check file presence / permissions
 * Ex: `root /var/www/; location /images {alias /var/www/assets/images};' 
 * 		-> server.getFullPath("/foo/bar", -1) -> "/var/www//foo/bar"
 * 		-> server.getFullPath("/images/foo.bar", loc) -> "/var/www/assets/images/foo.bar"
 */
std::string	ConfigServer::getFullPath(\
	const std::string &decoded_uri, \
	int location) const
{
	std::string			uri(decoded_uri);
	const std::string	&alias = this->getDirectiveParameter(location, "alias");

	if ("" == alias)
	{
		const std::string		&root = this->getDirectiveParameter(location, "root");
		if ("" != root)
		{
			/*
			if (false == uri.empty() && '/' == \
					*(uri.begin()) && '/' == *(root.end() - 1))
				uri.erase(uri.begin());
			*/
			uri = root + uri;
		}
	}
	else
	{
		if (0 > location || this->_locations.size() <= (unsigned int)location)
			return "";
		uri.replace(0, this->_locations[location].getUri().length(), alias);
	}
	return uri;
}

//

/* Gets the location from the decoded_uri
 * Returns -1 if the directives are to be found in the server block
 * Returns a unique identifier > -1 if found in a location block
 * ex `location /var/www/ {...;}'
 * 		-> server.getLocation("/") -> -1
 * 		-> server.getLocation("/var/www/foo.bar") -> location_id
 */
int	ConfigServer::getLocation(const std::string &decoded_uri) const
{
	const std::vector<ConfigLocation>	&locations = this->_locations;

	if (locations.empty())
		return -1;
	const std::vector<ConfigLocation>::const_iterator	&it_end = \
		locations.end();
	int i = 0;

	for (\
		std::vector<ConfigLocation>::const_iterator it = locations.begin(); \
		it_end != it; \
		++it)
	{
		if (it->inLocation(decoded_uri))
			return i;
		++i;
	}
	return -1;
}
