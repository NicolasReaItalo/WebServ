/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_post.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:46:13 by nrea              #+#    #+#             */
/*   Updated: 2024/10/07 14:52:43 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "headerParser.hpp"

header_infos Server::handle_post
(header_infos &response, ConfigServer  * config,
int locationIndex,std::map<std::string, std::string> header_attributes)
{
	//TODO nouvelle branche sur : multipart/form-data;boundary="boundary"
	FileInfos file;
	// On considere que c'est ue requete POST vers le PATH complet
	file.getInfos(response.ressourcePath);

	if (!file.valid_path())
	{
		{
			std::ostringstream oss;
			oss <<"[handle_post]	The ressource "<<response.ressourcePath <<" is forbidden";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return  response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
	}

	///<========== TODO ajouter le cas de CGI (une requete POST vers un script CGI existant)

	if (file.file_exist()) // si la ressource existe deja on ne va pas l'ecraser
	{
		{
			std::ostringstream oss;
			oss <<"[handle_post]	The ressource "<<response.ressourcePath <<" already exist";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return  response_error(HTTP_STATUS_CONFLICT, config, locationIndex);
	}

	// On test maintenant les droits sur le dossier parent
	std::string parent = getParentDir(response.ressourcePath);
	FileInfos parent_dir = FileInfos(parent);
	if ( !parent_dir.file_exist())
	{
		{
		std::ostringstream oss;
		oss <<"[handle_post]	:" <<parent;
		webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_INTERNAL_SERVER_ERROR, config, locationIndex);
	}
	{
		std::ostringstream oss;
		oss <<"[handle_post]	The parent directory of the ressource is :"<<parent;
		webservLogger.log(LVL_DEBUG, oss);
	}
	// On check les droits en ecriture/execution
	if (!parent_dir.write_permission() || !parent_dir.exec_permission())
	{
		{
			std::ostringstream oss;
			oss <<"[handle_post]	write and exec denied on :"<<parent;
			webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
	}

	// On test le chunk
	response.chunked =  header_attributes["Transfer-Encoding"] == "chunked";
	if (response.chunked)
	{
		{
			std::ostringstream oss;
			oss <<"[handle_post]	the Transfer-Encoding is set to chunked";
			webservLogger.log(LVL_DEBUG, oss);
		}
	}
	else
	{
		{
			std::ostringstream oss;
			oss <<"[handle_post]	the Transfer-Encoding is not set to chunked : comparing Content-Length to  body_size_max";
			webservLogger.log(LVL_DEBUG, oss);
		}
	}


	if (static_cast<unsigned long>(atol(header_attributes["Content-Length"].c_str())) >
	 static_cast<unsigned long>(atol(config->getDirectiveParameter(locationIndex, "client_max_body_size").c_str())))
	{
		{
			std::ostringstream oss;
			std::cout << static_cast<unsigned long>(atol(header_attributes["Content-Length"].c_str())) << std::endl << static_cast<unsigned long>(atol(config->getDirectiveParameter(locationIndex, "client_body_path").c_str())) << std::endl;;
			oss <<"[handle_post]	the Content-length exceed the max body size accepted for a non-chunked request";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_PAYLOAD_TOO_LARGE, config, locationIndex);
	}



// On verifie que le type du fichier recu est accepte par le server
	// if (header_attributes["Content-Type"] != "")
	// 	response.contentType = header_attributes["Content-Type"];
	// else
	// 	response.contentType = get_mime_type(response.ressourcePath);
	// if (!matchAcceptServerContentTypes(header_attributes["Content-Type"],mimeList))
	// 		return response_error(HTTP_STATUS_CONFLICT, config, locationIndex);

	response.toDo = POST;
	response.keepAlive = header_attributes["Connection"] == "keep-alive";
	response.returnCode = 201;
	// response pour initialiser ces champs vides
	response.interpreterPath = "";
	response.fd_ressource = 0;
	response.cgi_pid = 0;
	response.locationIndex = locationIndex;
	response.configServer = config;
	return response;
}
