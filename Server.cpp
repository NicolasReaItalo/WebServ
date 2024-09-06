/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 14:46:12 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/06 17:22:08 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, std::string host) : port(port), host(host)
{
	set_error_codes();
	set_mimeList();
	addrlen = sizeof(address);
    d_main = "/index.html";
    d_path = "";
}

void Server::set_error_codes()
{
    errorCodeList[400] = "bad request";
    errorCodeList[404] = "not found";
    errorCodeList[415] = "Unsupported Media Type";
}

void Server::set_mimeList()
{
	mimeList[".html"]   = "text/html";
    mimeList[".htm"]    = "text/html";
    mimeList[".png"]    = "image/png";
    mimeList[".jpg"]    = "image/jpg";
    mimeList[".jpeg"]   = "image/jpeg";
    mimeList[".gif"]    = "image/gif";
    mimeList[".ico"]    = "image/ico";
    mimeList[".css"]    = "text/css";
    mimeList[".js"]     = "application/javascript";
    mimeList[".json"]   = "application/json";
    mimeList[".pdf"]    = "application/pdf";
    mimeList[".css"]    = "text/css";
}