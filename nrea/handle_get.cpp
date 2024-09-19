/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_get.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 13:27:16 by nrea              #+#    #+#             */
/*   Updated: 2024/09/19 16:17:01 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"


std::string dummy_get_fullPath(ServerConfig &serverconfig, int locationIndex, std::string uri)
{
	(void) serverconfig;
	(void) locationIndex;
	return "./html-files" + uri;
}

const std::list<std::string> dummy_get_indexes(ServerConfig &serverconfig, int locationIndex)
{
	(void) serverconfig;
	(void) locationIndex;
	std::list<std::string> indexes;
	indexes.push_back("index.html");
	indexes.push_back("index.htm");
	indexes.push_back("index.php");
	return indexes;
}

bool dummy_get_autoindex_status(ServerConfig &serverconfig, int locationIndex)
{
	(void) serverconfig;
	(void) locationIndex;
	return false;
}

/*===================================================================================================*/
std::string get_extension(std::string uri)
{
	std::string wrong = "/."; //if found around '.' ==> not an extension
	std::string extension = "";
	size_t dot = uri.rfind(".");
	if ( dot != std::string::npos && dot && wrong.find(uri[dot-1]) == std::string::npos &&
		wrong.find(uri[dot+1]) == std::string::npos ) // a refactoriser pour eviter overflow dans le cas ou uri se finit par '.'
	{
		extension = uri.substr(dot + 1, uri.size() - dot - 1 );
	}
	return extension;
}

header_infos serve_regular_file
(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes)
{
	struct stat stat_buf;
	int ret;

	ret = stat(response.ressourcePath.c_str(),  &stat_buf);
	if (ret != 0)
	{
		int error = errno;
		if (ENOENT == error)
			response = response_error(HTTP_STATUS_NOT_FOUND, config, locationIndex);
		else if (EACCES == error)
			response = response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
		else
			response = response_error(HTTP_STATUS_INTERNAL_SERVER_ERROR, config, locationIndex);
		return response;
	}
	//on checke si le fichier est regulier et accesible en lecture
	//sinon on retourne erreur 403
	if (!S_ISREG(stat_buf.st_mode) || access(response.ressourcePath.c_str(), R_OK))
		return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);

	//==========TODO TODO TODO TODO==========================================================
	// On checke si le fichier est un CGI SI OUI// TODO =======> nouvelle branche
	std::string extension = get_extension(response.ressourcePath);
	webservLogger.log(LVL_DEBUG, "extension found [", extension,"]");


	// LA RESSOURCE N'EST PAS UN CGI
	//=======================================================================================
	//ON Ddetermine sa taille
	response.bodySize = GetFileSize(response.ressourcePath);


	///////TODO  DETERMINATION DU CHUNK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//Si la taille est trop grande on passe en chunked; //TODO
	unsigned long max_body_size = 12092300; // hardcode il faudra fair un appel a getDirectiveParameter(-, "client_max_body_size")
	// --------------------------------------


	if (response.bodySize > max_body_size)
		response.chunked = true;
	else
		response.chunked = false;

	response.returnCode = 200;
	response.locationIndex = locationIndex;
	response.configServer = &config;

	// response pour initialiser ces champs vides
	response.interpreterPath = "";
	response.fd_ressource = 0;
	response.cgi_pid = 0;

	// on DETERMINE LE  MIME TYPE DE LA RESSOURCE ==> a deplacer
	response.contentType = retrieve_mime_type(response.ressourcePath);

	//On verifie que le content-type est autorise
	if (!matchContentTypes(response.contentType, header_attributes["Accept"]))
		return response_error(HTTP_STATUS_NOT_ACCEPTABLE, config, locationIndex);
	///voir le keep-alive
	response.keepAlive = header_attributes["Connection"] == "keep-alive";
	return response;
}


header_infos handle_get
(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes)
{
	struct stat stat_buf;
	int ret;

	response.toDo = GET;

	////////TODO !!!!!
	//ON RECUPERE LE PATH COMPLET VERS LA RESSOURCE
	// response.ressourcePath = config.getFullPath(header_attributes["URI"], locationIndex);
	response.ressourcePath  = dummy_get_fullPath(config, locationIndex, header_attributes["URI"]);
	webservLogger.log(LVL_DEBUG, "full ressource path:", response.ressourcePath );

// test access
	ret = stat(response.ressourcePath.c_str(),  &stat_buf);
	if (ret != 0)
	{
		int error = errno;
		if (ENOENT == error)
			response = response_error(HTTP_STATUS_NOT_FOUND, config, locationIndex);
		else if (EACCES == error)
			response = response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
		else
			response = response_error(HTTP_STATUS_INTERNAL_SERVER_ERROR, config, locationIndex);
		return response;
	}

	//Si la ressource est un repertoire
	if (S_ISDIR(stat_buf.st_mode))
	{
		////// TODO TODO
		const std::list<std::string> indexes = dummy_get_indexes(config, locationIndex);
		// d'abord tester les differents index possibles
		for (std::list<std::string>::const_iterator it = indexes.begin(); it != indexes.end(); it++)
		{
			std::string full_path =response.ressourcePath;
			if (response.ressourcePath[response.ressourcePath.size() -1] != '/')
				full_path += "/";
			full_path +=  *it;
			if (access(full_path.c_str(), F_OK) == 0)
			{
				response.ressourcePath = full_path;
				webservLogger.log(LVL_DEBUG,"HeaderParser::found matching index value: ", *it);
				return serve_regular_file(response, config, locationIndex, header_attributes);
			}
		}
		//Si fichier regulier pas trouve
		webservLogger.log(LVL_DEBUG,"HeaderParser:: No index found - check if autoindex allowed");
		// verifer si autoindex autorise
		if (dummy_get_autoindex_status(config, locationIndex))
			//Oui -> serve_auto_index
			return response_autoindex(config, locationIndex, response);
		else
		{
			//Non -> erreur 403
			webservLogger.log(LVL_ERROR, "HeaderParser:: no index file found and autoindex forbidden for: ", response.ressourcePath);
			return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
		}
	}
	return serve_regular_file(response, config, locationIndex, header_attributes);
}

