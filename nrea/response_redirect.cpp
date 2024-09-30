/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_redirect.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:53:45 by nrea              #+#    #+#             */
/*   Updated: 2024/09/30 15:10:44 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

header_infos response_redirect(std::string  &return_code, std::string  &redir_url,
std::map<std::string, std::string> &header_attributes)
{
	header_infos response;
	response.toDo = RETURN;
	response.returnCode = std::atoi(return_code.c_str());
	response.ressourcePath = redir_url; // la location est stockee dans le ressource_path
	response.bodySize = 0; // content-length;
	response.chunked = false;
	response.keepAlive = (header_attributes["Connection"] == "keep-alive");
	response.configServer = NULL;
	response.locationIndex = -1;

	{
		std::ostringstream oss;
		oss <<"[HeaderParser]	RESPONSE  {"<<response.returnCode <<"} ";
		oss <<"{"<<str_todo(response.toDo)<<"}"<<"{"<<response.ressourcePath <<"}";
		if (response.keepAlive)
			oss<<"{keep-alive}";
		else
			oss<<"{close}";
		if (response.chunked)
			oss<<"{chunked}";
		else
			oss<<"{not chunked}";
		oss<<"";
		oss<<"{body-size: " << response.bodySize;
		oss<<"} ";
		oss<<"{content-type: " << response.contentType<<"}";
 		webservLogger.log(LVL_DEBUG, oss);
	}

	return response;
}

/*
Exemple de header de redirection:

HTTP/1.1 302 Found
Location: https://www.example.com/new-profile-url
Content-Length: 0

*/
