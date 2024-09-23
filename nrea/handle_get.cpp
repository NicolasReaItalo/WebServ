/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_get.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 13:27:16 by nrea              #+#    #+#             */
/*   Updated: 2024/09/23 12:48:16 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"


/*===================================================================================================*/

header_infos handleFileErrror(int error, header_infos &response, ConfigServer  & config,int locationIndex)
{
		{
			std::ostringstream oss;
			oss <<"[handleFileErrror]  "<<error ;
			webservLogger.log(LVL_DEBUG, oss);
		}
	if (ENOENT == error)
	{
		{
			std::ostringstream oss;
			oss <<"[handle_get] The ressource "<<response.ressourcePath <<" does not exist";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_NOT_FOUND, config, locationIndex);
	}
	else if (EACCES == error)
	{
		{
			std::ostringstream oss;
			oss <<"[handle_get] The ressource "<<response.ressourcePath <<" is forbidden";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return  response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
	}

	{
		std::ostringstream oss;
		oss <<"[handle_get] An internal error has occured";
		webservLogger.log(LVL_DEBUG, oss);
	}
	return  response_error(HTTP_STATUS_INTERNAL_SERVER_ERROR, config, locationIndex);
}



header_infos Server::serve_regular_file
(header_infos &response, ConfigServer  & config,int locationIndex,std::map<std::string, std::string> header_attributes)
{
	struct stat stat_buf;
	int ret;

	ret = stat(response.ressourcePath.c_str(),  &stat_buf);
	if (ret != 0)
		return handleFileErrror(errno,response,config, locationIndex);
	//on checke si le fichier est regulier et accessible en lecture
	//sinon on retourne erreur 403
	if (!S_ISREG(stat_buf.st_mode) || access(response.ressourcePath.c_str(), R_OK))
	{
		{
			std::ostringstream oss;
			oss <<"[serve_regular_file] "<<response.ressourcePath <<" is not a readable regular file";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
	}

	//==========TODO TODO TODO TODO==========================================================
	// On checke si le fichier est un CGI SI OUI// TODO =======> nouvelle branche
	std::string extension = getFileExtension(response.ressourcePath);
	//webservLogger.log(LVL_DEBUG, "HeaderParser:: extension found [", extension,"]");

	// LA RESSOURCE N'EST PAS UN CGI
	//=======================================================================================
	//ON Ddetermine sa taille
	response.bodySize = getFileSize(response.ressourcePath);

	///////TODO  DETERMINATION DU CHUNK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//Si la taille est trop grande on passe en chunked; //TODO
	// --------------------------------------

	if (response.bodySize > dummy_get_client_max_body_size(config, locationIndex))
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
	response.contentType = get_mime_type(response.ressourcePath);

	//On verifie que le content-type est autorise
	if (!matchContentTypes(response.contentType, header_attributes["Accept"]))
		return response_error(HTTP_STATUS_NOT_ACCEPTABLE, config, locationIndex);
	///voir le keep-alive
	response.keepAlive = header_attributes["Connection"] == "keep-alive";
	{
		std::ostringstream oss;
		oss <<"[HeaderParser] Response "<<response.returnCode <<" ";
		oss <<str_todo(response.toDo) <<" "<<response.ressourcePath;
		webservLogger.log(LVL_DEBUG, oss);
	}
	return response;
}


header_infos Server::handle_get
(header_infos &response, ConfigServer  & config,int locationIndex,std::map<std::string, std::string> header_attributes)
{
	struct stat stat_buf;
	int ret;

	response.toDo = GET;
	////////TODO !!!!!
	//ON RECUPERE LE PATH COMPLET VERS LA RESSOURCE
	// response.ressourcePath = config.getFullPath(header_attributes["URI"], locationIndex);
	response.ressourcePath  = dummy_get_fullPath(config, locationIndex, header_attributes["URI"]);
	{
		std::ostringstream oss;
		oss <<"[HeaderParser] retrieving full path from "<<"["<<header_attributes["URI"] <<"]-->["<<response.ressourcePath<<"]";
		webservLogger.log(LVL_DEBUG, oss);
	}

// test access
	ret = stat(response.ressourcePath.c_str(),  &stat_buf);
	if (ret != 0)
	return handleFileErrror(errno,response,config, locationIndex);

	//Si la ressource est un repertoire
	if (S_ISDIR(stat_buf.st_mode))
	{
			{
				std::ostringstream oss;
				oss <<"[handle_get] "<<response.ressourcePath<< "is a directory";
				webservLogger.log(LVL_DEBUG, oss);
			}
			{
				std::ostringstream oss;
				oss <<"[handle_get] "<<" looking for matching indexes with path "<<response.ressourcePath;
				webservLogger.log(LVL_DEBUG, oss);
			}
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
				{
					std::ostringstream oss;
					oss <<"[handle_get] "<<" Existing path with index found :" <<full_path;
					webservLogger.log(LVL_DEBUG, oss);
				}
				return serve_regular_file(response, config, locationIndex, header_attributes);
			}
		}
		//Si fichier regulier pas trouve
		// verifer si autoindex autorise

		{
			std::ostringstream oss;
			oss <<"[handle_get] "<<" No matching index found - checking if autoindex allowed in " <<response.ressourcePath ;
			webservLogger.log(LVL_DEBUG, oss);
		}

		if (dummy_get_autoindex_status(config, locationIndex))
		{
			{
				std::ostringstream oss;
				oss <<"[handle_get] "<<" autoindex allowed in "<<response.ressourcePath ;
				webservLogger.log(LVL_DEBUG, oss);
			}
			return response_autoindex(config, locationIndex, response);
		}
		else
		{
			{
				std::ostringstream oss;
				oss <<"[handle_get] "<<" autoindex forbidden in "<<response.ressourcePath ;
				webservLogger.log(LVL_DEBUG, oss);
			}
			return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
		}
	}
	return serve_regular_file(response, config, locationIndex, header_attributes);
}

