/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 14:15:21 by nrea              #+#    #+#             */
/*   Updated: 2024/10/10 11:41:46 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headerParser.hpp"

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

std::string str_todo(int todo)
{
	std::map<int, std::string> m;
	m[ERROR] = "ERROR";
	m[GET] = "GET";
	m[POST] = "POST";
	m[AUTOINDEX] = "AUTOINDEX";
	m[DELETE] = "DELETE";
	m[GET_CGI] = "GET_CGI";
	m[POST_CGI] = "POST_CGI";
	return m[todo];
}

/*Checks if the content-type of a file matches the accepted content-types of the client*/
bool matchAcceptContentTypes(std::string file_content_type, std::string accepted_types)
{
	if (accepted_types.find("*/*") != std::string::npos)
		return true;
	if (accepted_types.find(file_content_type) != std::string::npos)
		return true;
	//webservLogger.log(LVL_DEBUG, "HeaderParser not matching accepted  :", file_content_type);
	return false;
}
/*Checks if the content-type of the posted file matches the accepted content-types of the server*/

bool matchAcceptServerContentTypes(std::string file_content_type, std::map<std::string,std::string> mime_list)
{
	std::map<std::string, std::string>::iterator it;
	for (it = mime_list.begin(); it != mime_list.end(); it++)
	{
		if (it->second == file_content_type)
			return true;
	}
	return false;
}



char from_hex(char ch) {
    if (std::isdigit(ch)) {
        return ch - '0';
    } else if (std::isxdigit(ch)) {
        return std::tolower(ch) - 'a' + 10;
    }
    return -1;
}


// /%6E%6F%74%65%73/%6E%6F%74%65%73%2E%70%68%70 ==> /notes/notes.php
std::string url_decode(const std::string &url) {
    std::string result;
    for (std::size_t i = 0; i < url.size(); ++i) {
        if (url[i] == '%' && i + 2 < url.size() && isxdigit(url[i + 1]) && isxdigit(url[i + 2])) {
            char hex_char = from_hex(url[i + 1]) * 16 + from_hex(url[i + 2]);
            result += hex_char;
            i += 2;
        } else if (url[i] == '+') {
            result += ' ';
        } else {
            result += url[i];
        }
    }
    return result;
}

bool contains_forbbiden(std::string const &uri)
{
	std::string auth = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\
	abcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&()*+,;=%";

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

long getFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

std::string getFileExtension(std::string uri)
{
	std::string wrong = "/."; //if found around '.' ==> not an extension
	std::string extension = "";
	size_t dot = uri.rfind(".");
	if ( dot != std::string::npos && dot && wrong.find(uri[dot-1]) == std::string::npos &&
		dot + 1 > uri.size() && wrong.find(uri[dot+1]) == std::string::npos )
	{
		extension = uri.substr(dot + 1, uri.size() - dot - 1 );
	}
	return extension;
}

//return the parent directory in a file path
std::string getParentDir(std::string uri)
{
	std::size_t last_slash = uri.rfind('/');
	if (last_slash > 0)
	{
		return uri.substr(0, last_slash);
	}
	return std::string("/forbidden"); // dummy so an error would be catch later;
}

// ABCDEFGHIJ
// 0123456789

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

/*########   DEBUG UTILITIES    ###########################################*/

void displayHeaderInfos(header_infos const &info)
{
	std::map<int, std::string> todo;
	todo[ERROR] = "ERROR";
	todo[GET] = "GET";
	todo[POST] = "POST";
	todo[DELETE] = "DELETE";
	todo[GET_CGI] = "GET_CGI";
	todo[POST_CGI] = "POST_CGI";


	std::cout<< GREEN<< "===== CHAMPS SET PAR HEADER INFOS ================================="<<std::endl;
	std::cout<< BLUE<< "action to perform: "<<YELLOW<<"["<<RST<< todo[info.toDo] <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "return code: "<<YELLOW<<"["<<RST<< info.returnCode <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "body size: "<<YELLOW<<"["<<RST<< info.bodySize <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "chunked ?: "<<YELLOW<<"["<<RST<< info.chunked <<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "keep alive ?: "<<YELLOW<<"["<<RST<< info.keepAlive<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "ressource path: "<<YELLOW<<"["<<RST<< info.ressourcePath<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "content type: "<<YELLOW<<"["<<RST<< info.contentType<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "interpreter path: "<<YELLOW<<"["<<RST<< info.interpreterPath<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "location Index: "<<YELLOW<<"["<<RST<< info.locationIndex<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "Server config pointer: "<<YELLOW<<"["<<RST<< info.configServer<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "time stamp : "<<YELLOW<<"["<<RST<< info.timestamp<<YELLOW<<"]"<<RST<<std::endl;
	std::cout<< BLUE<< "fd ressource: "<<YELLOW<<"["<<RST<< info.fd_ressource<<YELLOW<<"]"<<RST<<std::endl;

}





/*##############################################################*/
