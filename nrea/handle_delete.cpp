/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_delete.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 11:18:45 by nrea              #+#    #+#             */
/*   Updated: 2024/10/08 13:31:34 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

header_infos Server::handle_delete(header_infos &response, ConfigServer  * config,
int locationIndex, std::map<std::string, std::string> header_attributes)
{
	(void) header_attributes;
		{
			std::ostringstream oss;
			oss <<"[handle_delete]	checking the file"
			<<"["<<response.ressourcePath<<"]";
			webservLogger.log(LVL_DEBUG, oss);
		}
	FileInfos file(response.ressourcePath);
	if (!file.valid_path())
	{
		{
			std::ostringstream oss;
			oss <<"[handle_delete]	the path to "
			<<"["<<response.ressourcePath<<"]  iss forbidden";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
	}
	if (!file.file_exist())
	{
		{
			std::ostringstream oss;
			oss <<"[handle_delete]	the ressource"
			<<"["<<response.ressourcePath<<"]  does'nt exist";
			webservLogger.log(LVL_DEBUG, oss);
		}
		return response_error(HTTP_STATUS_NOT_FOUND, config, locationIndex);
	}
		{
			std::ostringstream oss;
			oss <<"[handle_delete]	path is valid and file exist...Checking parent dir";
			webservLogger.log(LVL_DEBUG, oss);
		}
	FileInfos parent_dir(getParentDir(response.ressourcePath));
	if (!(parent_dir.exec_permission() && parent_dir.write_permission())
		&& parent_dir.read_permission())
		{
			{
				std::ostringstream oss;
				oss <<"[handle_delete]	We don't have enough rights on directory "
				<<"["<<getParentDir(response.ressourcePath)<<"]";
				webservLogger.log(LVL_DEBUG, oss);
			}
			return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
		}
	response.toDo = DELETE;
	response.returnCode = 204; //SUCCESS NO CONTENT
	response.locationIndex = locationIndex;
	response.configServer = config;
	response.keepAlive = false;
	response.chunked = false;

	// response pour initialiser ces champs vides
	response.interpreterPath = "";
	response.fd_ressource = 0;
	response.cgi_pid = 0;

	// on DETERMINE LE  MIME TYPE DE LA RESSOURCE ==> a deplacer
	response.contentType = get_mime_type(response.ressourcePath);
	return response;
}
