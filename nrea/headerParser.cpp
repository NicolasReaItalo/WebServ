/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 16:20:48 by nrea              #+#    #+#             */
/*   Updated: 2024/09/18 17:02:53 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"



/*########    UTILITIES                                                 ###########################################*/



void displayHeaderInfos(header_infos const &info)
{
	std::map<int, std::string> todo;
	todo[ERROR] = "ERROR";
	todo[GET] = "GET";
	todo[POST] = "POST";
	todo[DELETE] = "DELETE";
	todo[GET_CGI] = "GET_CGI";
	todo[POST_CGI] = "POST_CGI";


	std::cout<< GREEN<< "===== CHAMPS SET PAR HEADER INFOS ================================="<<std::endl;
	std::cout<< BLUE<< "action to perform: "<<YELLOW<<"["<<RST<< todo[info.toDo] <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "return code: "<<YELLOW<<"["<<RST<< info.returnCode <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "body size: "<<YELLOW<<"["<<RST<< info.bodySize <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "chunked ?: "<<YELLOW<<"["<<RST<< info.chunked <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "keep alive ?: "<<YELLOW<<"["<<RST<< info.keepAlive<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "ressource path: "<<YELLOW<<"["<<RST<< info.ressourcePath<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "content type: "<<YELLOW<<"["<<RST<< info.contentType<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "interpreter path: "<<YELLOW<<"["<<RST<< info.interpreterPath<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "query parameters : "<<YELLOW<<"["<<RST<< info.queryParams<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "location Index: "<<YELLOW<<"["<<RST<< info.locationIndex<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "Server config pointer: "<<YELLOW<<"["<<RST<< info.configServer<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "time stamp : "<<YELLOW<<"["<<RST<< info.timestamp<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "fd ressource: "<<YELLOW<<"["<<RST<< info.fd_ressource<<YELLOW<<"]"<<RST<<std::endl;



}

/*#################################################################################################################################*/

/* returns a header_infos set to the appropriate error*/
header_infos response_error(std::string error_code, ServerConfig  & config,int locationIndex)
{
	header_infos response;

	response.toDo = ERROR;
	response.returnCode = std::atoi(error_code.c_str());//error_code;
	response.contentType = "text/html";
	// response.ressourcePath =  config.getCustomErrorPage(locationIndex,std::atoi(error_code.c_str())); //config.getCustomErrorPage(locationIndex, error_code);
	response.ressourcePath = "./generic-error.html"; // pour TEST
	response.bodySize = GetFileSize(response.ressourcePath.c_str());
	response.configServer = &config;
	response.locationIndex = locationIndex;

	//la caracteristique keep-alive depend de l'erreur:  a ameliorer pour la rendre + generale
	response.keepAlive = error_code != HTTP_STATUS_BAD_REQUEST && error_code != HTTP_STATUS_INTERNAL_SERVER_ERROR;
	return response;
}


header_infos Server::headerParser(std::string rawBuffer, std::pair<std::string, std::string> interface)
{
	header_infos response;
	std::vector<std::string> splitted_buffer;
	std::map<std::string, std::string> header_attributes;



	ServerConfig defaultconfig; ///ServerConfig par defaut pour les messages d'erreurs
	int locationIndex = 0; // defaultconfig config

	splitted_buffer = splitString(rawBuffer, "\r\n");
	if (splitted_buffer.size() < 3)
		return response_error(HTTP_STATUS_BAD_REQUEST, defaultconfig, locationIndex);


	std::vector<std::string> tmp = splitString(splitted_buffer[0], " ");
	if (tmp.size() != 3)
		response_error(HTTP_STATUS_BAD_REQUEST, defaultconfig, locationIndex);
	header_attributes["Method"] = tmp[0];
	header_attributes["Raw_URI"] = tmp[1];
	header_attributes["Protocol"] = tmp[2];

/// ON RECUPERE CHAQUE ATTRIBUT DU HEADER dans une map

	std::vector<std::string>::iterator it;
	for (it = splitted_buffer.begin() + 1; it != splitted_buffer.end(); it++)
	{
		if (!(*it).size())
			break ;
		tmp = splitString(*it, ": ");
		if (tmp.size() == 2)
			header_attributes[tmp[0]] = tmp[1];
	}
//-----------------------

// ON VERIFIE QUE HOST EST PRESENT
	std::map<std::string,std::string>::iterator h_it = header_attributes.find("Host");
	if (h_it == header_attributes.end())
		return response_error(HTTP_STATUS_BAD_REQUEST, defaultconfig, locationIndex);
//---------------------------------------------------------------------

//ON SPLITTE LE HOST POUR RECUPERER LE PORT SI PRECISE
	tmp = splitString(header_attributes["Host"], ":");
	if (tmp.size() == 2)
	{
		if (! contains_only_numeric(tmp[1]))
			return response_error(HTTP_STATUS_BAD_REQUEST, defaultconfig, locationIndex);
		header_attributes["Host"] = tmp[0];
		header_attributes["Port"] = tmp[1];
	}
	else if ( tmp.size() != 1)
		return response_error(HTTP_STATUS_BAD_REQUEST, defaultconfig, locationIndex);


/// Maintenant qu'on a le host on peut recuperer le
// bon server de config et on set sa location!
	// ON RECUPERE LE BON SERVER

	const ServerConfig * serverconfig = find_server(interface,header_attributes["host"]);

	//DEBUG
	std::cout<< BLUE << "address:"<<RST <<serverconfig->getAddress()<<std::endl;
	std::cout<< BLUE << "Port:"<<RST <<serverconfig->getPort()<<std::endl;


//TODO TODO TODO
// ON RECUPERE LA LOCATION ( CACHE )
	// On verra quand ca marchera.                           =====================> TODO !!!!




///VERIFICATION DU PROTOCOLE------------------
	if (header_attributes["Protocol"] != "HTTP/1.1")
		return response_error(HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED, const_cast<ServerConfig&>(*serverconfig), locationIndex);
////---------------------------------------



///ON VERIFIE QUE L'URI N'EST PAS VIDE ET COMMENCE PAR '/'-------------------------------------------
	if (header_attributes["Raw_URI"].size() == 0 || header_attributes["Raw_URI"][0] != '/')
		return response_error(HTTP_STATUS_BAD_REQUEST, const_cast<ServerConfig&>(*serverconfig), locationIndex);
///---------------------------------------------------------------------------------------------------

///CONVERSION DES CARACTERES SPECIAUX DE L'URI---------------------------------------------
	try
	{
		header_attributes["Converted_URI"] = convert_uri(header_attributes["Raw_URI"]);
	}
	catch(const std::exception& e)
	{
		return response_error(HTTP_STATUS_BAD_REQUEST, const_cast<ServerConfig&>(*serverconfig), locationIndex);
	}
//--------------------------------------------------------------------------------------------

///ON RECUPERE UNE EVENTUELLE QUERY--------------------------------------------
	// on reutilise tmp pour split l'uri
	tmp = splitString(header_attributes["Converted_URI"], "?");
	if (tmp.size() == 2 )
	{
		header_attributes["URI"] = tmp[0];
		header_attributes["Query"] = tmp[1];
	}
	else if (tmp.size() != 1)
		return response_error(HTTP_STATUS_BAD_REQUEST, const_cast<ServerConfig&>(*serverconfig), locationIndex);
	else
		header_attributes["URI"] = tmp[0];

///------------------------------------------------------------------------------


// // ON VERIFIE QUE LA METHODE EST AUTORISEE ------------------------------- DESACTIVE en attendant serverconfig
// 	if (!defaultconfig.inDirectiveParameters(locationIndex, "limit", header_attributes["Method"]))
// 		return response_error(HTTP_STATUS_METHOD_NOT_ALLOWED, const_cast<ServerConfig&>(*serverconfig), locationIndex);
// //---------------------------------------------------



// ON SWITCHE SELON LA METHODE---------------------------------

	if (header_attributes["Method"] == "GET")
		response = handle_get(response, defaultconfig, locationIndex, header_attributes);
	// else if (header_attributes["Method"] == "POST")
	// 	response = handle_post(response, defaultconfig, locationIndex, header_attributes);
	// else if (header_attributes["Method"] == "DELETE")
	// 	response = handle_delete(response, defaultconfig, locationIndex, header_attributes);

//-------------------------------------------------------------

//DEBUG------------------------------------------------------------------

	std::cout<<std::endl<<BLUE<<"------------------REQUEST HEADER------------------------"<<RST<<std::endl;
	std::cout<< rawBuffer;
	std::cout<<std::endl<<BLUE<<"--------------------------------------------------------"<<RST<<std::endl;
	displayHeaderInfos(response);

// /--------------------------DEBUG----------------------------------

	return response;
}



// header_infos handle_post(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes)
// {
// 	(void) config;
// 	(void) locationIndex;
// 	(void) header_attributes;

// 	return response;
// }
// header_infos handle_delete(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes)
// {
// 	(void) config;
// 	(void) locationIndex;
// 	(void) header_attributes;
// 	return response;
// }



