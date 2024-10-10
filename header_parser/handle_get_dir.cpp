/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_get_dir_dir.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 11:23:19 by nrea              #+#    #+#             */
/*   Updated: 2024/09/27 11:23:52 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"



header_infos Server::handle_dir(header_infos &response,
 ConfigServer  * config,int locationIndex,std::map<std::string, std::string> &header_attributes)
 {
	// config->_debug_print();
	{
		std::ostringstream oss;
		oss <<"[handle_get_dir]	"<<response.ressourcePath<< "is a directory";
		webservLogger.log(LVL_DEBUG, oss);
	}
	if (config->inDirectives(locationIndex, "index") == true)
	{
		{
		std::ostringstream oss;
		oss <<"[handle_get_dir]	matching index authorized in :"<<response.ressourcePath;
		webservLogger.log(LVL_DEBUG, oss);
		}
		{
			std::ostringstream oss;
			oss <<"[handle_get_dir]	"<<"looking for matching indexes with path "<<response.ressourcePath;
			webservLogger.log(LVL_DEBUG, oss);
		}
		ConfigBlock::parameters_t  indexes = config->getDirectiveParameters(locationIndex, "index");

		// const std::list<std::string> indexes = dummy_get_indexes();
		// d'abord tester les differents index possibles
		for (ConfigBlock::parameters_t ::const_iterator it = indexes.begin(); it != indexes.end(); it++)
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
					oss <<"[handle_get_dir]	"<<"Existing path with index found :" <<full_path;
					webservLogger.log(LVL_DEBUG, oss);
				}
				std::vector<std::string> cgi_ext;
				cgi_ext.push_back(".php");
				cgi_ext.push_back(".py");
				std::string cgi = detect_cgi(response.ressourcePath);
				if (cgi != "") // une extension cgi a ete detecte das l'uri
				{
					{
						std::ostringstream oss;
						oss <<"[handle_get_dir]	cgi file detected";
						webservLogger.log(LVL_DEBUG, oss);
					}
					return handle_get_cgi(response,cgi,config, locationIndex, header_attributes);
				}


				return serve_regular_file(response, config, locationIndex, header_attributes);
			}
		}





	}
	else
	{
		{
			std::ostringstream oss;
			oss <<"[handle_get_dir]	"<<" no index directive for "<<response.ressourcePath;
			webservLogger.log(LVL_DEBUG, oss);
		}

			}

		//Si fichier regulier pas trouve ou pas de directive index
		// verifer si autoindex autorise

		{
			std::ostringstream oss;
			oss <<"[handle_get_dir]	"<<
			" checking if autoindex allowed in " <<response.ressourcePath ;
			webservLogger.log(LVL_DEBUG, oss);
		}
		if (config->getDirectiveParameter(locationIndex, "autoindex") == "on")
		{
			{
				std::ostringstream oss;
				oss <<"[handle_get_dir]	"<<" autoindex allowed in "<<response.ressourcePath ;
				webservLogger.log(LVL_DEBUG, oss);
			}
			response.uri = header_attributes["URI"];
			return response_autoindex(config, locationIndex, response);
		}
		else
		{
			{
				std::ostringstream oss;
				oss <<"[handle_get_dir]	"<<" autoindex forbidden in "<<response.ressourcePath ;
				webservLogger.log(LVL_DEBUG, oss);
			}
			return response_error(HTTP_STATUS_FORBIDDEN, config, locationIndex);
		}

 }
