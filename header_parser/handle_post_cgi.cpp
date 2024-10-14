/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_post_cgi.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:05:47 by nrea              #+#    #+#             */
/*   Updated: 2024/10/14 14:50:48 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

header_infos handle_post_cgi(header_infos & response, std::string cgi,
ConfigServer *config,int locationIndex, std::map<std::string,std::string> &header_attributes)
{
	{
		std::ostringstream oss;
		oss <<"[handle_cgi]	entering handle_get_cgi()";
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
		oss <<"[handle_cgi]	 checking existence and permissions of script  "<<
		response.ressourcePath;
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

	response.chunked =  header_attributes["Transfer-Encoding"] == "chunked";
	if (response.chunked)
	{
		{
			std::ostringstream oss;
			oss <<"[handle_post_cgi]	the Transfer-Encoding is set to chunked";
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

	if (static_cast<unsigned long>(atol(header_attributes["Content-Length"].c_str())) >
	 static_cast<unsigned long>(atol(config->getDirectiveParameter(locationIndex, "client_max_body_size").c_str())))
	{
		{
			std::ostringstream oss;
			oss <<"[handle_post]	the Content-length exceed the max body size accepted for a non-chunked request";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_PAYLOAD_TOO_LARGE, config, locationIndex);
	}


	}


	// On prepare notre environement;

	response.envMap["SERVER_SOFTWARE"]= "webserv";
	response.envMap["REQUEST_METHOD"]= "POST";
	response.envMap["SERVER_NAME"]= config->getAddress();
	response.envMap["SERVER_PORT"]= config->getPort();
	response.envMap["SERVER_PROTOCOL"]= "HTTP/1.1";
	response.envMap["CONTENT_LENGTH"]= header_attributes["Content-Length"];
	response.envMap["CONTENT_TYPE"]= header_attributes["Content-Type"];
	// header_attributes["Content-Type"] = "text/html";
	response.envMap["SCRIPT_NAME"]= response.ressourcePath;
	response.envMap["PATH_TRANSLATED"]= response.ressourcePath;
	response.envMap["QUERY_STRING"]= header_attributes["QUERY_STRING"];
	response.envMap["PATH_INFO"]= path_info;

	std::map<std::string,std::string>::iterator it;
	for (it = header_attributes.begin(); it != header_attributes.end(); it++)
		response.envMap[cgi_convert(it->first)] = it->second;

	// et notre reponse
	response.toDo = POST_CGI;
	response.returnCode = 200;
	response.bodySize = static_cast<unsigned long>(atol(header_attributes["Content-Length"].c_str()));
	response.contentType = header_attributes["Content-Type"];
	response.interpreterPath = interpreter;
	return response;
}
