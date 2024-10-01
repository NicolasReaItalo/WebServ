/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/01 13:35:31 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBlock.hpp"
#include "ConfigServer.hpp"

ConfigServer::ConfigServer(void) {};
ConfigServer::~ConfigServer(void) {};

ConfigServer::ConfigServer(const ConfigServer &other) :
	ConfigBlock(other)
{
	*this = other;
}

ConfigServer &ConfigServer::operator=(const ConfigServer &other)
{
	if (&other != this)
	{
		ConfigBlock::operator=(other);
		this->_address = other._address;
		this->_port = other._port;
		this->_server_names = other._server_names;
		this->_locations = other._locations;
	}
	return *this;
}
