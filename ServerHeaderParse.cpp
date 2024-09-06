/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHeaderParse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 10:24:13 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/06 15:55:11 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sstream>


std::map<std::string, std::string> Server::headerRequestParser(const std::string& request)
{
	std::string::size_type idx = request.find("\r\n\r\n");
	std::string header = request.substr(0, idx + 3);	
	std::istringstream request_stream(header);
    std::string method;
    request_stream >> method;
	headerRequest["method"] = method;
	std::string address;
	request_stream >> address;
	headerRequest["address"] = address;
	while (header.length() > 1)
	{
		idx = header.find("\r");
		header = header.substr(idx + 2);
		int step = header.find(": ");
		if (header.length() > 1)
			headerRequest[header.substr(0, step)] = header.substr(step + 2, header.find("\r") - step);
	}
	// for(std::map<std::string, std::string >::const_iterator it = headerRequest.begin();
    // it != headerRequest.end(); ++it)
	// {
	// 	std::cout << it->first << " AND " << it->second << "\n";
	// }
	return headerRequest;
}
