/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_get.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 13:27:16 by nrea              #+#    #+#             */
/*   Updated: 2024/09/18 17:26:42 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

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

	// LA RESSOURCE N'EST PAS UN CGI
	//=======================================================================================


	//ON Ddetermine sa taille
	response.bodySize = GetFileSize(response.ressourcePath);


	///////TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//Si la taille est trop grande on passe en chunked; //TODO
	unsigned long max_body_size = 12092300; // hardcode il faudra fair un appel a getDirectiveParameter(-, "client_max_body_size")
	//--------------------------------------


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
	std::string temp_root = "/home/nrea/Documents/PROJETS/webserv/github/html-files";

	response.ressourcePath  = temp_root + header_attributes["URI"]; //<<== temporaire pour tests
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
		// d'abord tester les differents index possibles
		// Trouve ? ===> serve_regular_file
		//Si fichier regulier pas trouve
		// verifer si autoindex autorise
		//Oui -> declencher autoindex et updater ressourcePath + relancer stat
				//===> serve_regular_file
		//Non -> erreur 403
		return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex); // temporaire
	}
	return serve_regular_file(response, config, locationIndex, header_attributes);
}

