/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_error.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 10:13:31 by nrea              #+#    #+#             */
/*   Updated: 2024/09/19 11:56:07 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

/* returns a header_infos set to the appropriate error*/
header_infos response_error(std::string error_code, ServerConfig  & config,int locationIndex)
{
	header_infos response;

	response.toDo = ERROR;
	response.returnCode = std::atoi(error_code.c_str());//error_code;
	response.contentType = "text/html";
	// response.ressourcePath =  config.getCustomErrorPage(locationIndex,std::atoi(error_code.c_str())); //config.getCustomErrorPage(locationIndex, error_code);
	response.ressourcePath = "./html-files/generic-error.html"; // pour TEST
	response.bodySize = GetFileSize(response.ressourcePath.c_str());
	response.configServer = &config;
	response.locationIndex = locationIndex;

	//la caracteristique keep-alive depend de l'erreur:  a ameliorer pour la rendre + generale
	response.keepAlive = error_code != HTTP_STATUS_BAD_REQUEST && error_code != HTTP_STATUS_INTERNAL_SERVER_ERROR;
	return response;
}
