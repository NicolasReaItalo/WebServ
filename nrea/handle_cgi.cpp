/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_cgi.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 17:35:59 by nrea              #+#    #+#             */
/*   Updated: 2024/10/01 14:21:44 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

header_infos handle_cgi(header_infos & response, std::string cgi, ConfigServer *config,
int locationIndex, std::map<std::string,std::string> &header_attributes)
{
	{
		std::ostringstream oss;
		oss <<"[handle_cgi]	entering handle_cgi()";
		webservLogger.log(LVL_DEBUG, oss);
	}


	//On verifie que ce cgi est autorise
	std::string interpreter = config->getDirectiveOutput(locationIndex, "cgi", cgi);
	{
	std::ostringstream oss;
	oss <<"[handle_cgi]	cgi matching"<<cgi<<" with intepreter "<<interpreter;
	webservLogger.log(LVL_DEBUG, oss);
	}
	if (interpreter.empty())
	{
		{
			std::ostringstream oss;
			oss <<"[handle_cgi]	cgi of type "<<cgi<<" are forbidden in this location";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
	}
	// ON verifie que le path vers l'interpreter existe et est executable ce serait bete
	FileInfos interpreter_path(interpreter);
	if (!interpreter_path.file_exist() || !interpreter_path.exec_permission())
	{
		{
			std::ostringstream oss;
			oss <<"[handle_cgi]	path to the interpreter does not exist or is not executable";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_INTERNAL_SERVER_ERROR, config, locationIndex);
	}
	// strip d'un eventuel PATH_INFO
	{
		std::ostringstream oss;
		oss <<"[handle_cgi]	stripping the PATH_INFO";
		webservLogger.log(LVL_DEBUG, oss);
	}
	std::string path_info = "";
	stripPathInfo(response.ressourcePath,path_info,cgi);
	// Le script existe et est autorise en lecture ?
	FileInfos script_path(response.ressourcePath);
	{
		std::ostringstream oss;
		oss <<"[handle_cgi]	 checking existence and permissions of script  "<<response.ressourcePath;
		webservLogger.log(LVL_DEBUG, oss);
	}
	if (!script_path.file_exist())
	{
		{
			std::ostringstream oss;
			oss <<"[handle_cgi]	the script doesn't exist "<<response.ressourcePath;
			webservLogger.log(LVL_DEBUG, oss);
		}

		return response_error(HTTP_STATUS_NOT_FOUND, config, locationIndex);
	}
	{
		std::ostringstream oss;
		oss <<"[handle_cgi]	 "<<response.ressourcePath <<" exists";
		webservLogger.log(LVL_DEBUG, oss);
	}
	if (!script_path.read_permission())
		return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
	{
		std::ostringstream oss;
		oss <<"[handle_cgi]	 "<<response.ressourcePath <<" is readable";
		webservLogger.log(LVL_DEBUG, oss);
	}

	// On prepare notre environement;

	response.envMap["SERVER_SOFTWARE"]= "webserv";
	response.envMap["REQUEST_METHOD"]= "GET";
	response.envMap["PATH_INFO"]= path_info;
	response.envMap["SERVER_NAME"]= config->getAddress();
	response.envMap["SERVER_PORT"]= config->getPort();
	response.envMap["SERVER_PROTOCOL"]= "HTTP/1.1";
	response.envMap["CONTENT_LENGTH"]= "0";
	response.envMap["SCRIPT_NAME"]= response.ressourcePath;
	response.envMap["PATH_TRANSLATED"]= response.ressourcePath;
	response.envMap["QUERY_STRING"]= header_attributes["QUERY_STRING"];

	std::map<std::string,std::string>::iterator it;
	for (it = header_attributes.begin(); it != header_attributes.end(); it++)
		response.envMap[cgi_convert(it->first)] = it->second;

	// et notre reponse
	response.toDo = GET_CGI;
	response.returnCode = 200;
	response.bodySize = 0;
	response.interpreterPath = interpreter;
	return response;
}

/*
Rules for converting HTTP headers into CGI environment variables:

Prefix: Each HTTP header is prefixed with HTTP_.
Uppercase: The name of the header is converted to uppercase.
Hyphens (-): Hyphens in the header names are replaced with underscores (_).
Environment Variable Format: After following these transformations,
the resulting string becomes an environment variable that is accessible to the CGI script.

Example Conversions:
The header Host: example.com becomes HTTP_HOST=example.com.
The header User-Agent: Mozilla/5.0 becomes HTTP_USER_AGENT=Mozilla/5.0.
The header Accept-Language: en-US,en;q=0.5 becomes HTTP_ACCEPT_LANGUAGE=en-US,en;q=0.5.

*/



/*

Variables spécifiques à la requête

PATH_TRANSLATED
Contient le chemin demandé par le client après que les conversions virtuel → physique ont été faites par le serveur.
SCRIPT_NAME
Le chemin virtuel vers le script étant exécuté. Exemple : « /cgi-bin/script.cgi »
REMOTE_HOST
Le nom d'hôte du client. Si le serveur ne possède pas cette information (par exemple, lorsque la résolution DNS inverse est désactivée), REMOTE_HOST sera vide.
REMOTE_ADDR
L'adresse IP du client.
AUTH_TYPE
Le type d'identification utilisé pour protéger le script (s’il est protégé et si le serveur supporte l'identification).
REMOTE_USER
Le nom d'utilisateur du client, si le script est protégé et si le serveur supporte l'identification.
REMOTE_IDENT
Nom d'utilisateur (distant) du client faisant la requête. Le serveur doit supporter l'identification RFC 931. Cette variable devrait être utilisée à des fins de journaux seulement.
CONTENT_TYPE
Le type de contenu attaché à la requête, si des données sont attachées (comme lorsqu'un formulaire est envoyé avec la méthode « POST »).
CONTENT_LENGTH
La longueur du contenu envoyé par le client.
Variables provenant du client

*/
