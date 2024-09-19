/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 16:20:48 by nrea              #+#    #+#             */
/*   Updated: 2024/09/19 19:02:45 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <Server.hpp>

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <set>
#include <exception>
#include "ServerConfig.hpp"
#include "Server.hpp"
#include <sys/stat.h>
#include "http_errors.hpp"
#define RST		"\033[0m"
#define RED		"\033[1;31m"
#define GREEN	"\033[1;32m"
#define YELLOW	"\033[1;33m"
#define BLUE	"\033[1;34m"





/*########    UTILITIES                                                 ###########################################*/


std::vector<std::string> splitString(const std::string& str, std::string delimiter)
{
	std::vector<std::string> tokens;
	std::size_t start = 0;
	std::size_t end = str.find(delimiter);

	while (end != std::string::npos)
	{
		tokens.push_back(str.substr(start, end - start));
		start = end + delimiter.size();
		end = str.find(delimiter, start);
	}
	tokens.push_back(str.substr(start));
	return tokens;
}

//remove special characters
std::string  convert_uri(std::string const &uri)
{
	std::string converted = uri;
	std::map<std::string, std::string> conversion;
	conversion["%20"] = " ";
	conversion["%21"] = "!";
	conversion["%22"] = "\"";
	conversion["%23"] = "#";
	conversion["%24"] = "$";
	conversion["%25"] = "%";
	conversion["%26"] = "&";
	conversion["%27"] = "'";
	conversion["%28"] = "(";
	conversion["%29"] = ")";
	conversion["%2A"] = "*";
	conversion["%2B"] = "+";
	conversion["%2C"] = ",";
	conversion["%2D"] = "-";
	conversion["%2E"] = ".";
	conversion["%2F"] = "/";
	conversion["%3A"] = ":";
	conversion["%3B"] = ";";
	conversion["%3C"] = "<";
	conversion["%3D"] = "=";
	conversion["%3E"] = ">";
	conversion["%3F"] = "?";
	conversion["%40"] = "@";
	conversion["%5B"] = "[";
	conversion["%5C"] = "\\";
	conversion["%5D"] = "]";
	conversion["%5E"] = "^";
	conversion["%5F"] = "_";
	conversion["%60"] = "`";
	conversion["%7B"] = "{";
	conversion["%7D"] = "}";
	conversion["%7E"] = "~";

	std::size_t pos = converted.find('%');
	while (pos != std::string::npos)
	{
		if (pos + 2 >= converted.size())
		{
			throw std::runtime_error("BAD REQUEST");
		}

		std::string code = converted.substr(pos, 3);
		std::map<std::string, std::string>::iterator it = conversion.find(code);
		if (it != conversion.end())
		{
			converted.replace(pos, 3, it->second);
		}
		else
		{
			throw std::runtime_error("BAD REQUEST");
		}
		pos = converted.find('%', pos + it->second.length());
	}
	// cas du +
	pos = converted.find('+');
	while (pos != std::string::npos)
	{
		converted.replace(pos, 1, " ");
		pos = converted.find('%', pos + 1);
	}

	return converted;
}

bool contains_forbbiden(std::string const &uri)
{
	std::string auth = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\
	abcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";

	for(std::string::const_iterator it = uri.begin(); it != uri.end(); ++it)
	{
		if ( auth.find(*it) == std::string::npos)
		{
			std::cout << "["<<*it<<"]"<<std::endl;
			return true;
		}
	}
	return false;
}


long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


bool contains_only_numeric(std::string str)
{
	std::string numeric = "0123456789";
	std::string::iterator it;
	for (it = str.begin(); it != str.end(); it++)
	{
		if (numeric.find(*it) == std::string::npos)
			return false;
	}
	return true;
}
/*#################################################################################################################################*/




// header_infos handle_get(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes);
// header_infos handle_post(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes);
// header_infos handle_delete(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes);





/* */
header_infos return_error(std::string error_code, ServerConfig  & config,int locationIndex)
{
	header_infos response;

	(void)error_code;
	response.toDo = ERROR;
	response.returnCode = 0;//error_code;
	response.contentType = "text/html";
	response.ressourcePath = config.getCustomErrorPage(locationIndex, 0); //config.getCustomErrorPage(locationIndex, error_code);
	response.bodySize = GetFileSize(response.ressourcePath.c_str());
	response.configServer = &config;
	response.locationIndex = locationIndex;

	//la caracteristique keep-alive depend de l'erreur:  a ameliorer pour la rendre + generale
	response.keepAlive = error_code != "400" && error_code != "512";
	return response;
}
#include <sstream>
header_infos Server::headerParser(std::string rawBuffer, std::pair<std::string, std::string> address)
{
	 (void)address;
	 (void)rawBuffer;
	header_infos response;

	std::istringstream stream(rawBuffer);
    std::string line;

	std::string method;
    std::string uri;
    // Get the first line of the HTTP request
    if (std::getline(stream, line)) {
        std::istringstream lineStream(line);
        
        // Extract method, URI, and ignore HTTP version
        lineStream >> method >> uri;
    }

///--------------------------DEBUG----------------------------------
	if (method == "GET")
		response.toDo = GET;
	if (method == "POST")
		response.toDo = POST;
	if (method == "DELETE")
		response.toDo = DELETE;
	response.chunked = false;
	response.interpreterPath = "";
	response.contentType = get_mime_type(uri);
	response.ressourcePath = uri;
	// response.toDo = POST;
	response.keepAlive = false;
	response.bodySize = GetFileSize(uri);
	response.returnCode = 0;
	response.cgi_pid = 0;
	response.configServer = NULL;
	response.fd_ressource = -1;
	response.locationIndex = 0;
	response.queryParams = "";
	response.timestamp = 0;
	// std::cout << "method = " << response.toDo << "\nuri = " << response.ressourcePath << "\nsize = " << response.bodySize<< std::endl;
	
	return response;
}




// header_infos handle_get(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes)
// {
// 	(void) header_attributes;
// 	//ON RECUPERE LE PATH COMPLET VERS LA RESSOURCE
// 	response.ressourcePath = config.getFullPath(header_attributes["uri"], locationIndex);


// 	return response;
// }






// header_infos handle_post(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes)
// {
// 	(void) config;
// 	(void) locationIndex;
// 	(void) header_attributes;

// 	return response;
// }
// header_infos handle_delete(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes)
// {
// 	(void) config;
// 	(void) locationIndex;
// 	(void) header_attributes;
// 	return response;
// }




void displayHeaderInfos(header_infos const &info)
{
	std::map<int, std::string> todo;
	todo[1] = "ERROR";
	todo[2] = "GET";
	todo[3] = "POST";
	todo[4] = "DELETE";
	todo[5] = "GET_CGI";
	todo[6] = "POST_CGI";


	std::cout<< GREEN<< "===== CHAMPS SET PAR HEADER INFOS ================================="<<std::endl;
	std::cout<< BLUE<< "action to perform: "<<YELLOW<<"["<<RST<< todo[info.toDo] <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "return code: "<<YELLOW<<"["<<RST<< info.returnCode <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "body size: "<<YELLOW<<"["<<RST<< info.bodySize <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "chunked ?: "<<YELLOW<<"["<<RST<< info.chunked <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "keep alive ?: "<<YELLOW<<"["<<RST<< info.keepAlive<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "ressource path: "<<YELLOW<<"["<<RST<< info.ressourcePath<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "content type: "<<YELLOW<<"["<<RST<< info.contentType<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "interpreter path: "<<YELLOW<<"["<<RST<< info.interpreterPath<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "query parameters : "<<YELLOW<<"["<<RST<< info.queryParams<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "location Index: "<<YELLOW<<"["<<RST<< info.locationIndex<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "Server config pointer: "<<YELLOW<<"["<<RST<< info.configServer<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< GREEN<< "===== CHAMPS LAISSES LIBRES================================="<<std::endl;
	std::cout<< BLUE<< "time stamp : "<<YELLOW<<"["<<RST<< info.timestamp<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "fd ressource: "<<YELLOW<<"["<<RST<< info.fd_ressource<<YELLOW<<"]"<<RST<<std::endl;



}




// int main()
// {
// 	std::string ok_req = "GEhT /my%20directory/index.php?name=TOTO+lasticot&age=10 HTTP/1.1\r\n\
// Host: www.example.org\r\n\
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n\
// Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3\r\n\
// Accept-Encoding: gzip, deflate, br\r\n\
// Alt-Used: www.rfc-editor.org\r\n\
// Connection: keep-alive\r\n\r\n";

// 	displayHeaderInfos(headerParser(ok_req));

// 	return 0;
// }



