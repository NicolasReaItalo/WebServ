/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 10:52:33 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/20 12:21:38 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigLocation.hpp"


ConfigLocation::ConfigLocation(void) {};

ConfigLocation::ConfigLocation(const std::string &uri) : _uri(uri) {};

ConfigLocation::~ConfigLocation(void) {};

const std::string	&ConfigLocation::getUri(void) const
{
	return (this->_uri);
}

bool	ConfigLocation::inLocation(const std::string& decoded_uri) const
{
	return (0 == decoded_uri.find(this->_uri));
}

ConfigLocation::ConfigLocation(const ConfigLocation &other)
{
	*this = other;
}

ConfigLocation &ConfigLocation::operator=(const ConfigLocation &other)
{
	if (&other != this)
	{
		ConfigBlock::operator=(other);
		this->_uri = other._uri;
	}
	return (*this);
}

