/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_responses.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 13:26:40 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/02 13:37:53 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
/*continue*/
const char *resp_100()
{
	std::string ret = "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 58\r\n"
            "\r\n"
            "<html><body><h1>404 Not Found</h1><p>Resource not found.</p></body></html>";

	return ret.c_str();
}
/*processing*/
const char *resp_102()
{
	std::string ret = "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 58\r\n"
            "\r\n"
            "<html><body><h1>404 Not Found</h1><p>Resource not found.</p></body></html>";

	return ret.c_str();
}
/*ok*/
const char *resp_200(std::string request)
{
}
/*created*/
const char *resp_201(std::string request)
{
}

/*bad request*/
const char *resp_400(std::string request)
{
}

/*unauthorized*/
const char *resp_401(std::string request)
{
}

/*forbidden*/
const char *resp_403(std::string request)
{
}

/*not found*/
const char *resp_404(std::string request)
{
}

/*method not allowe*/
const char *resp_405(std::string request)
{
}

/*request timeout*/
const char *resp_408(std::string request)
{
}

/*method not implemented*/
const char *resp_501(std::string request)
{
}

/*HTTP version not supported*/
const char *resp_505(std::string request)
{
}