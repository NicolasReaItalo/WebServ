/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer_eval.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/14 10:02:06 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBlock.hpp"
#include "ConfigServer.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include "config_constants.h"

static int	_atoi_strict(const std::string &num, std::size_t n)
{
	const std::size_t 	len = num.length();

	if (0 == len || len > n)
		return -1;
	char				c;
	int					value;

	value = 0;
	for (std::size_t i = 0; i < len; ++i)
	{
		c = num[i];
		if ('0' > c || '9' < c)
			return -1;
		value = 10 * value + (c - '0');
	}
	return value;
}

static bool _is_address(const std::string &address)
{
	const int			max_ip_num = 255;
	const std::size_t	max_ip_num_len = 3;
	const int			fields_size = 4;
	const char			ip_separator = '.';

	std::stringstream	ss(address);
	std::string			s255;
	int					val255;
	int					length;

	length = 0;
	while (std::getline(ss, s255, ip_separator) && (fields_size + 1) != length)
	{
		val255 = _atoi_strict(s255, max_ip_num_len);
		if (val255 < 0 || val255 > max_ip_num)
			return false;
		++length;
	}
	return (fields_size == length);
}

static bool _is_port(const std::string &port)
{
	const int			max_port = 65535;
	const std::size_t	max_port_len = 5;
	const int			val65535 = _atoi_strict(port, max_port_len);

	return (val65535 > 0 && val65535 <= max_port);
}

//Evals
int	ConfigServer::_evalServerName(void)
{
	if (false == this->ConfigBlock::inDirectives("server_name"))
		return CF_ERRDMISSING;
	ConfigBlock::parameters_t &nameParameter = this->_directive_parameters["server_name"];
	if (nameParameter.empty())
		return CF_ERRDPARAM;
	this->_server_names = nameParameter;
	return 0;
}

int	ConfigServer::_evalListen(void)
{
	if (!this->ConfigBlock::inDirectives("listen"))
		return CF_ERRDMISSING;
	parameters_t &listenParameter = this->_directive_parameters["listen"];
	if (1 != listenParameter.size())
		return CF_ERRDPARAM;
	std::stringstream		ss(listenParameter.front());
	std::string				elem;
	int						i = 0;
	while (std::getline(ss, elem, ':') && i != 3)
	{
		if (0 == i && _is_address(elem))
			this->_address = elem;
		else if (1 == i && _is_port(elem))
			this->_port = elem;
		else
			return CF_ERRDPARAM;
		++i;
	}
	if (2 != i)
		return CF_ERRDPARAM;
	return 0;
}
