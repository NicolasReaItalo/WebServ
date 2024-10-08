/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_error_page.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:30 by qgiraux           #+#    #+#             */
/*   Updated: 2024/10/08 10:44:49 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include "Server.hpp"

std::string Server::generate_error_page(int errcode)
{
    std::stringstream os;

    os << "<!DOCTYPE html> \n"
    << "<html lang=\"en\"> \n"
    << "<head>\n"
    << "<meta charset=\"UTF-8\"> \n"
    << "<meta name=\"viewport\"\n"
    << "content=\"width=device-width, initial-scale=1.0\"> \n"
    << "<title> \n"
    << errcode <<  errorList[errcode]
    << "\n</title> \n"
    << "<link rel=\"stylesheet\" \n"
    << "href=\"/style.css\"> \n"
    << "</head>\n"
    << "<body>\n"
    << "<div class=\"page\"> \n"
    << "<div class=\"title\"> \n"
    << "<h1>" << errcode << "</h1> \n"
    << "</div> \n"
    << "<div class=\"title\"><p>\n"
    << errorList[errcode]
    << "\n</p></div> \n"
    << "<div class=\"title\">\n"
    << " <a href=\"/index.html\">\n"
    << "Go Back to Home \n"
    << "</a></div> \n"
    << " <div class=\"bloc\">\n"
	<< "<img src=\"https://http.cat/images/" << errcode << ".jpg\" alt=" << errcode <<" class=\"resizable-image\">\n"
	<< "	</div></div></body> </html>\n";
    return os.str();
}