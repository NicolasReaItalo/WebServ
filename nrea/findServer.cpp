/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   findServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 15:47:38 by nrea              #+#    #+#             */
/*   Updated: 2024/09/20 15:31:12 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"


/*Finds the appropriate ConfigServer according to Address, Port and Host
Returns a ConfigServer * pointer  */
ConfigServer * Server::findServer(std::pair<std::string, std::string> interface, std::string host)
{
	std::list<ConfigServer>::iterator server_it;
	std::vector<ConfigServer*> matching_servers;

	// On recupere tous les server qui matchent en address:port
	for (server_it = servers.begin(); server_it != servers.end(); server_it++)
	{
		ConfigServer &server = *server_it;
		if (server.getAddress() == interface.first && server.getPort() == interface.second)
			matching_servers.push_back(&server);

	}
	{
		std::ostringstream oss;
		oss <<"[findServer]	more than one match for "<< interface.first<<":"<< interface.second << " checking server_names";
		webservLogger.log(LVL_DEBUG, oss);
	}
	// Si il y en a plus d'un on cherche une valeur server_name qui matche
	std::vector<ConfigServer*>::iterator matching_it;
	for (matching_it = matching_servers.begin(); matching_it != matching_servers.end(); matching_it++)
	{
		const ConfigServer::parameters_t names = (*matching_it)->getServerNames();
		ConfigServer::parameters_t::const_iterator name_it = names.begin();
		for (;name_it != names.end(); name_it++)
		{
			if (*name_it == host)
			{
				{
					std::ostringstream oss;
					oss <<"[findServer]	The Host match with servername "<<host ;
					webservLogger.log(LVL_DEBUG, oss);
				}
				return *matching_it;
			}
		}
	}
	// Si aucun server_name ne matche on renvoie le premier de la liste par defaut
	{
		std::ostringstream oss;
		oss <<"[findServer]	no match found for Host <"<<host <<"> returning default server";
		webservLogger.log(LVL_DEBUG, oss);
	}

	return matching_servers[0];
}
