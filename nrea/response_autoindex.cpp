/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_autoindex.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 15:25:39 by nrea              #+#    #+#             */
/*   Updated: 2024/09/20 14:33:56 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

header_infos response_autoindex(ConfigServer &config, int locationIndex, header_infos response)
{
	response.toDo = AUTOINDEX;
	response.returnCode = 200;
	response.contentType = "text/html";
	response.configServer = &config;
	response.locationIndex = locationIndex;

	return response;
}
