/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer_getters.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/01 12:56:19 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigServer.hpp"
#include <string>

/* Gets address
 * from `listen [address]:[port];'
 */
const std::string				&ConfigServer::getAddress(void) const
{
	return this->_address;
}

/* Gets port
 * from `listen [address]:[port];'
 */
const std::string				&ConfigServer::getPort(void) const
{
	return this->_port;
}

/* Gets the server names
 * from `server_name [...];'
 */
const ConfigServer::parameters_t	&ConfigServer::getServerNames(void) const
{
	return this->_server_names;
}
