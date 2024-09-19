/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_error_page.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 12:49:30 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/19 12:49:31 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include "Server.hpp"

std::string Server::generate_error_page(int errcode)
{
    std::stringstream os;

    os << "<!DOCTYPE html> "
    << "<html lang=\"en\"> "
    << "<head>"
    << "<meta charset=\"UTF-8\"> "
    << "<meta name=\"viewport\"" 
    << "content=\"width=device-width, initial-scale=1.0\"> "
    << "<title> "
    << errcode <<  errorList[errcode]
    << "</title> "
    << "<link rel=\"stylesheet\" " 
    << "href=\"style.css\"> "
    << "</head>" 
    << "<body>"
    << "<div class=\"error-wrapper\"> "
    << "<div class=\"error-title\"> "
    << "<h1>" << errcode << "</h1> "
    << "</div> "
    << "<div class=\"error-title\"><p>"
    << errorList[errcode]
    << "</p></div> "
    << "<div class=\"error-title\">"
    << " <a href=\"index.html\">" 
    << "Go Back to Home "
    << "</a></div> "
    << " <div class=\"error-container\">"
	<< "<img src=\"images/404.png\" alt=" << errcode <<" class=\"resizable-image\">"
	<< "	</div></div></body> </html>";
    return os.str();
}