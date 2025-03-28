/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_index_page.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 12:07:33 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/10 13:12:31 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sstream>

std::string Server::generate_index_page(std::map<std::string, std::string> index, const header_infos& header)
{
    std::stringstream os;

    os << "<!DOCTYPE html> \n"
    << "<html lang=\"en\"> \n"
    << "<head>\n"
    << "<meta charset=\"UTF-8\">\n"
    << "<meta name=\"viewport\"\n" 
    << "content=\"width=device-width, initial-scale=1.0\">\n"
    << "<title>\n"
    << "Index\n"
    << "</title>\n"
    << "</head>\n" 
    << "<body>\n"
    << "Index of " << header.uri << "<br><br>\n";
    for (std::map<std::string, std::string>::iterator it = index.begin(); it != index.end(); ++it)
    {
        if (*it->second.rbegin() == '/')
            os << "<img src=\"/images/folder.png\" alt=\"folder icon\" style=\"height: 1em; vertical-align: middle;\">\n";
        else
            os << "<img src=\"/images/file.png\" alt=\"file icon\" style=\"height: 1em; vertical-align: middle;\">\n";
            
        os << "<A HREF = \"" << it->second << "\">" << it->first << "</A><br>\n" ;
    }
    os << "</body>\n</html>\n";
    return os.str();
}