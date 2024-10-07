/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chunk_receive.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:25:30 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/07 13:42:32 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::chunked_post(int fd, std::vector<unsigned char> body, header_infos& header)
{
    int i = 0;
    std::string str = (char*)&(body[0]);
    while (i < body.size())
    {
        int chunk_size = 0;
    }
}