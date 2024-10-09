/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_autoindex.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 15:25:39 by nrea              #+#    #+#             */
/*   Updated: 2024/10/08 13:36:38 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

header_infos response_autoindex(ConfigServer *config, int locationIndex, header_infos response)
{
	response.toDo = AUTOINDEX;
	response.returnCode = 200;
	response.contentType = "text/html";
	response.configServer = config;
	response.locationIndex = locationIndex;
	response.chunked = false;
	response.bodySize = 0;
	return response;
}
