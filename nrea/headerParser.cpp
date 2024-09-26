/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 16:20:48 by nrea              #+#    #+#             */
/*   Updated: 2024/09/24 15:44:51 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"


/*split all the attributes of the raw header buffer and returns a map of all attributes
this function can throw a RuntimeError("bad request") exception*/
std::map<std::string, std::string> split_buffer(std::string rawBuffer)
{
	std::map<std::string, std::string> header_attributes;
	std::vector<std::string> splitted_buffer;

	splitted_buffer = splitString(rawBuffer, "\r\n");
	if (splitted_buffer.size() < 2)
		throw std::runtime_error("bad request");

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
	ConfigServer defaultconfig; ///ConfigServer par defaut pour les messages d'erreurs en cas de pb de parsing
	int locationIndex = 0; // defaultconfig location
	{
	std::ostringstream oss;
	oss <<"[HeaderParser] A new request header has been received "<< interface.first<<":"<< interface.second
	<<std::endl<<rawBuffer;
	webservLogger.log(LVL_INFO, oss);
	}

	try
	{
		header_attributes = split_buffer(rawBuffer);
	}
	catch(const std::runtime_error& e)
	{
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] The request header is not properly formatted";
		webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_BAD_REQUEST, defaultconfig, locationIndex);
	}

// ON VERIFIE QUE HOST EST PRESENT
	std::map<std::string,std::string>::iterator h_it = header_attributes.find("Host");
	if (h_it == header_attributes.end())
	{
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] The request header is not properly formatted";
		webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_BAD_REQUEST, defaultconfig, locationIndex);
	}

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
	{
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] The request header is not properly formatted";
		webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_BAD_REQUEST, defaultconfig, locationIndex);
	}

/// Maintenant qu'on a le host on peut recuperer le
// bon server de config et on set sa location!
	// ON RECUPERE LE BON SERVER
	//webservLogger.log(LVL_DEBUG,"HeaderParser:: looking up for the serverConfig:",interface.first, interface.second, header_attributes["Host"]);
	ConfigServer * serverconfig = findServer(interface,header_attributes["Host"]);
	{
	std::ostringstream oss;
	oss <<"[HeaderParser] findServer() ==> "<<serverconfig;
	webservLogger.log(LVL_DEBUG, oss);
	}

///VERIFICATION DU PROTOCOLE------------------
	if (header_attributes["Protocol"] != "HTTP/1.1")
	{
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] The protocol is not supported ==> "<<header_attributes["Protocol"];
		webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED, const_cast<ConfigServer&>(*serverconfig), locationIndex);
	}
////---------------------------------------

///ON VERIFIE QUE L'URI N'EST PAS VIDE ET COMMENCE PAR '/'-------------------------------------------
	if (header_attributes["Raw_URI"].size() == 0 || header_attributes["Raw_URI"][0] != '/')
	{
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] The uri is empty or dos not start with '/' ==> "<<header_attributes["Raw_URI"];
		webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_BAD_REQUEST, const_cast<ConfigServer&>(*serverconfig), locationIndex);
	}
///---------------------------------------------------------------------------------------------------

///CONVERSION DES CARACTERES SPECIAUX DE L'URI---------------------------------------------
	try
	{
		header_attributes["Converted_URI"] = convert_uri(header_attributes["Raw_URI"]);
	}
	catch(const std::exception& e)
	{
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] The uri contains forbidden special characters  ==> "<<header_attributes["Raw_URI"];
		webservLogger.log(LVL_DEBUG, oss);
		}

		return response_error(HTTP_STATUS_BAD_REQUEST, const_cast<ConfigServer&>(*serverconfig), locationIndex);
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
	{
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] The URI is not properly formatted ==> "<<header_attributes["Raw_URI"];
		webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_BAD_REQUEST, const_cast<ConfigServer&>(*serverconfig), locationIndex);
	}
	else
		header_attributes["URI"] = tmp[0];

///------------------------------------------------------------------------------
// ON RECUPERE LA LOCATION pour la mettre en cache pour les appels suivant a configServer
	locationIndex = serverconfig->getLocation(header_attributes["URI"]);
	{
	std::ostringstream oss;
	oss <<"[HeaderParser] get_location("<< header_attributes["URI"]<<") ==> "<<locationIndex;
	webservLogger.log(LVL_DEBUG, oss);
	}

//TODO TODO TODO
//	// Si il existe un return dans la location on redirige directement
	if (serverconfig->inDirectives(locationIndex,"return"))
	{
		ConfigBlock::parameters_t  test = serverconfig->getDirectiveParameters(locationIndex, "return");
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] return found  "<<test.front()<< " | "<< test.back() ;
		webservLogger.log(LVL_DEBUG, oss);
		}
	}

// // ON VERIFIE QUE LA METHODE EST AUTORISEE ------------------------------- DESACTIVE en attendant serverconfig
	if (serverconfig->inDirectiveParameters(locationIndex,"limit",header_attributes["Method"]))
	{
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] The method  "<<header_attributes["Method"] << " is forbidden";
		webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_METHOD_NOT_ALLOWED, const_cast<ConfigServer&>(*serverconfig), locationIndex);
	}
	{
	std::ostringstream oss;
	oss <<"[HeaderParser] The method  "<<header_attributes["Method"] << " is authorized";
	webservLogger.log(LVL_DEBUG, oss);
	}
// //---------------------------------------------------

////////TODO !!!!!
	//ON RECUPERE LE PATH COMPLET VERS LA RESSOURCE
	// response.ressourcePath = config.getFullPath(header_attributes["URI"], locationIndex);
	response.ressourcePath  = dummy_get_fullPath(*serverconfig, locationIndex, header_attributes["URI"]);
	{
		std::ostringstream oss;
		oss <<"[HeaderParser] retrieving full path from "
		<<"["<<header_attributes["URI"] <<"]-->["<<response.ressourcePath<<"]";
		webservLogger.log(LVL_DEBUG, oss);
	}

// ON SWITCHE SELON LA METHODE---------------------------------

	if (header_attributes["Method"] == "GET")
	{
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] GET method detected ==> starting handle_get()";
		webservLogger.log(LVL_DEBUG, oss);
		}
		response = handle_get(response, defaultconfig, locationIndex, header_attributes);
	}
	else if (header_attributes["Method"] == "POST")
	{
		{
		std::ostringstream oss;
		oss <<"[HeaderParser] POST method detected ==> starting handle_post()";
		webservLogger.log(LVL_DEBUG, oss);
		}
		response = handle_post(response, defaultconfig, locationIndex, header_attributes);
	}
	else if (header_attributes["Method"] == "DELETE")
		response = handle_delete(response, defaultconfig, locationIndex, header_attributes);
	else
		return response_error(HTTP_STATUS_METHOD_NOT_ALLOWED, const_cast<ConfigServer&>(*serverconfig), locationIndex);
	{
		std::ostringstream oss;
		oss <<"[HeaderParser] RESPONSE  {"<<response.returnCode <<"} ";
		oss <<"{"<<str_todo(response.toDo)<<"}"<<"{"<<response.ressourcePath <<"}";
		if (response.keepAlive)
			oss<<"{keep-alive}";
		else
			oss<<"{close}";
		oss <<"\n                                              ";
		if (response.chunked)
			oss<<"{chunked}";
		else
			oss<<"{not chunked}";
		oss<<"";
		oss<<"{body-size: " << response.bodySize;
		oss<<"} ";
		oss<<"{content-type: " << response.contentType<<"}";
 		webservLogger.log(LVL_DEBUG, oss);
	}
	return response;
}




