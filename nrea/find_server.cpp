/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 15:47:38 by nrea              #+#    #+#             */
/*   Updated: 2024/09/20 14:36:32 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"


/*Finds the appropriate ConfigServer according to Address, Port and Host
Returns a ConfigServer * pointer  */
const ConfigServer * Server::find_server(std::pair<std::string, std::string> interface, std::string host)
{
	std::list<ConfigServer>::iterator server_it;
	std::vector<const ConfigServer*> matching_servers;

	// On recupere tous les server qui matchent en address:port
	for (server_it = servers.begin(); server_it != servers.end(); server_it++)
	{
		const ConfigServer &server = *server_it;
		if (server.getAddress() == interface.first && server.getPort() == interface.second)
			matching_servers.push_back(&server);

	}
	// Si il y en a plus d'un on cherche une valeur server_name qui matche
	std::vector<const ConfigServer*>::iterator matching_it;
	for (matching_it = matching_servers.begin(); matching_it != matching_servers.end(); matching_it++)
	{
		const ConfigServer::parameters_t names = (*matching_it)->getServerNames();
		ConfigServer::parameters_t::const_iterator name_it = names.begin();
		for (;name_it != names.end(); name_it++)
		{
			if (*name_it == host)
				return *matching_it;
		}
	}
	// Si aucun server_name ne matche on renvoie le premier de la liste par defaut
	return matching_servers[0];
}
