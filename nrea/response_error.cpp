/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_error.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 10:13:31 by nrea              #+#    #+#             */
/*   Updated: 2024/09/20 17:13:02 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

std::string dummy_get_customerrorPage(void)
{
	// la vraie fonction renvoie un path complet ou "" si
	// il faut generer une erreur
	return "";
}

/* returns a header_infos set to the appropriate error*/
header_infos response_error(std::string error_code, ConfigServer  & config,int locationIndex)
{
	header_infos response;
	response.toDo = ERROR;
	response.returnCode = std::atoi(error_code.c_str());//error_code;
	response.contentType = "text/html";
	// response.ressourcePath =  config.getCustomErrorPage(locationIndex,std::atoi(error_code.c_str())); //config.getCustomErrorPage(locationIndex, error_code);
	response.ressourcePath = dummy_get_customerrorPage(); // pour TEST
	if (response.ressourcePath.size())
		response.bodySize = getFileSize(response.ressourcePath.c_str());
	response.configServer = &config;
	response.locationIndex = locationIndex;

	//la caracteristique keep-alive depend de l'erreur:  a ameliorer pour la rendre + generale
	response.keepAlive = error_code != HTTP_STATUS_BAD_REQUEST && error_code != HTTP_STATUS_INTERNAL_SERVER_ERROR;
	{
		std::ostringstream oss;
		oss <<"[HeaderParser] Response "<<response.returnCode <<" ";
		oss <<str_todo(response.toDo) <<" "<<response.ressourcePath;
		webservLogger.log(LVL_DEBUG, oss);
	}
	return response;
}
