/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerStop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:55 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/19 12:49:56 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::closeAllFd()
{
    std::map<int, std::pair<std::string, std::string> >::iterator it;

    for (it = fd_set.begin(); it != fd_set.end(); it++)
        close(it->first);
}