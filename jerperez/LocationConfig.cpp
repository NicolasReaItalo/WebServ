/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 10:52:33 by jerperez          #+#    #+#             */
/*   Updated: 2024/09/13 11:51:03 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"


const std::string	&LocationConfig::getUri(void) const
{
	return (this->_uri);
}

bool	LocationConfig::inLocation(const std::string& decoded_uri)
{
	return (0 == decoded_uri.find(this->_uri));
}

LocationConfig::LocationConfig(const LocationConfig &other)
{
	*this = other;
}

LocationConfig &LocationConfig::operator=(const LocationConfig &other)
{
	if (&other != this)
	{
		BlockSimpleConfig::operator=(other); //
		this->_uri = other._uri;
	}
	return (*this);
}

// void	LocationConfig::setUri(const std::string& uri)
// {
// 	this->_uri = uri;
// }
