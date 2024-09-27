/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dummy.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 12:44:08 by nrea              #+#    #+#             */
/*   Updated: 2024/09/27 14:07:55 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*

FONCTION DUMMY A REMPLACER PAR LES VRAIES
*/
// std::string dummy_get_fullPath(ConfigServer &serverconfig, int locationIndex, std::string uri);
const std::list<std::string> dummy_get_indexes();
bool dummy_get_autoindex_status(ConfigServer &serverconfig, int locationIndex);
unsigned long dummy_get_client_max_body_size(void);
bool dummy_isAuthorized(ConfigServer const *serverconfig, int locationIndex, std::string method);
bool dummy_isReturn(ConfigServer const *serverconfig, int locationIndex, std::string method);
std::string dummy_server_accepted_types(void);
