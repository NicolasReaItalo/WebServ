/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 16:20:48 by nrea              #+#    #+#             */
/*   Updated: 2024/09/19 12:12:28 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

/*split all the attributes of the raw header buffer and returns a map of all attributes
this function can throw a RuntimeError("bad request") exception*/
std::map<std::string, std::string> split_buffer(std::string rawBuffer)
{
	std::map<std::string, std::string> header_attributes;
	std::vector<std::string> splitted_buffer;

	webservLogger.log(LVL_DEBUG, "HeaderParser :: split_buffer()");

	splitted_buffer = splitString(rawBuffer, "\r\n");
	if (splitted_buffer.size() < 2)
	{
		webservLogger.log(LVL_DEBUG, "splitted_buffer.size()", splitted_buffer.size());
		throw std::runtime_error("bad request");
	}


	std::vector<std::string> tmp = splitString(splitted_buffer[0], " ");
	if (tmp.size() != 3)
		throw std::runtime_error("bad request");
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

	return header_attributes;
}



header_infos Server::headerParser(std::string rawBuffer, std::pair<std::string, std::string> interface)
{
	header_infos response;
	std::map<std::string, std::string> header_attributes;
	std::vector<std::string> tmp;
	ServerConfig defaultconfig; ///ServerConfig par defaut pour les messages d'erreurs en cas de pb de parsing
	int locationIndex = 0; // defaultconfig location


	webservLogger.log(LVL_INFO, "HeaderParser:: A new request header has been received", interface.first, interface.second);
	// webservLogger.log(LVL_DEBUG,"raw header [",rawBuffer,"]");

try
{
	header_attributes = split_buffer(rawBuffer);
}
catch(const std::runtime_error& e)
{
	webservLogger.log(LVL_ERROR, "An error has been encountered during split_buffer()");
	return response_error(HTTP_STATUS_BAD_REQUEST, defaultconfig, locationIndex);
}

// ON VERIFIE QUE HOST EST PRESENT
	std::map<std::string,std::string>::iterator h_it = header_attributes.find("Host");
	if (h_it == header_attributes.end())
		return response_error(HTTP_STATUS_BAD_REQUEST, defaultconfig, locationIndex);

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

	webservLogger.log(LVL_DEBUG,"HeaderParser:: looking up for the serverConfig:",interface.first, interface.second, header_attributes["Host"]);
	const ServerConfig * serverconfig = find_server(interface,header_attributes["Host"]);

//TODO TODO TODO
// ON RECUPERE LA LOCATION ( CACHE )
	// On verra quand ca marchera.                           =====================> TODO !!!!

///VERIFICATION DU PROTOCOLE------------------
	if (header_attributes["Protocol"] != "HTTP/1.1")
	{
		webservLogger.log(LVL_ERROR, "protocol not supported", header_attributes["Protocol"]);
		return response_error(HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED, const_cast<ServerConfig&>(*serverconfig), locationIndex);
	}
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
		webservLogger.log(LVL_ERROR, "forbidden character converter", header_attributes["Raw_URI"]);
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
	{
		webservLogger.log(LVL_DEBUG, "HeaderParser: GET method has been detected ==> handle_get()");
		response = handle_get(response, defaultconfig, locationIndex, header_attributes);
	}
	// else if (header_attributes["Method"] == "POST")
	// 	response = handle_post(response, defaultconfig, locationIndex, header_attributes);
	// else if (header_attributes["Method"] == "DELETE")
	// 	response = handle_delete(response, defaultconfig, locationIndex, header_attributes);

//-------------------------------------------------------------

	webservLogger.log(LVL_DEBUG, "HeaderParser: returning response", response.ressourcePath );
	return response;
}




