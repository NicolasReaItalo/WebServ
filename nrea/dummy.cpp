/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dummy.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 12:46:44 by nrea              #+#    #+#             */
/*   Updated: 2024/09/27 14:07:44 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dummy.hpp"

bool dummy_isAuthorized(ConfigServer const *serverconfig, int locationIndex, std::string method)
{
	(void) serverconfig;
	(void) method;
	(void) locationIndex;
	return true;
}

bool dummy_isReturn(ConfigServer const *serverconfig, int locationIndex, std::string method)
{
	(void) serverconfig;
	(void) method;
	(void) locationIndex;
	return true;
}


// std::string dummy_get_fullPath(ConfigServer &serverconfig, int locationIndex, std::string uri)
// {
// 	(void) serverconfig;
// 	(void) locationIndex;
// 	return "./html-files" + uri;
// }

const std::list<std::string> dummy_get_indexes(void)
{
	std::list<std::string> indexes;
	indexes.push_back("index.html");
	indexes.push_back("index.htm");
	indexes.push_back("index.php");
	return indexes;
}

bool dummy_get_autoindex_status(ConfigServer &serverconfig, int locationIndex)
{
	(void) serverconfig;
	(void) locationIndex;
	return true;
}

unsigned long dummy_get_client_max_body_size()
{

	return 1000000;
}
std::string dummy_server_accepted_types(void)
{
	return "*/*";
}
