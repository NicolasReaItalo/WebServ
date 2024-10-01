/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer_eval.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:29:48 by jerperez          #+#    #+#             */
/*   Updated: 2024/10/01 14:07:40 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBlock.hpp"
#include "ConfigServer.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include "config_error_constants.h"

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
	while (std::getline(ss, elem, ':'))
	{
		if (0 == i)
			this->_address = elem;
		else if (1 == i)
			this->_port = elem;
		else
			return CF_ERRDPARAM;
		++i;
	}
	if (2 != i)
		return CF_ERRDPARAM;
	return 0;
}
