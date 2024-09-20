/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constructor.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jerperez <jerperez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:28 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/20 14:33:56 by jerperez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


Server::Server(std::list<ConfigServer> servers) : servers(servers)
{
    list_size = servers.size();
    std::vector<int> vect(list_size);
    std::vector<struct sockaddr_in> vect2(list_size);
    server_fd = vect;
    address = vect2;
    nfds = 0;
    maxBodySize = BUFFER_SIZE;
    set_mimeList();
    set_errorList();

}