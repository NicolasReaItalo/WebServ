/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_responses.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 13:26:40 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/03 14:08:23 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
// /*continue*/
// const char *resp_100()
// {
// 	std::string ret = "HTTP/1.1 404 Not Found\r\n"
//             "Content-Type: text/html\r\n"
//             "Content-Length: 58\r\n"
//             "\r\n"
//             "<html><body><h1>404 Not Found</h1><p>Resource not found.</p></body></html>";

// 	return ret.c_str();
// }
// /*processing*/
// const char *resp_102()
// {
// 	std::string ret = "HTTP/1.1 404 Not Found\r\n"
//             "Content-Type: text/html\r\n"
//             "Content-Length: 58\r\n"
//             "\r\n"
//             "<html><body><h1>404 Not Found</h1><p>Resource not found.</p></body></html>";

// 	return ret.c_str();
// }
// /*ok*/
// const char *resp_200(std::string request)
// {
// }
// /*created*/
// const char *resp_201(std::string request)
// {
// }

/*bad request*/
void Server::resp_400(std::string request, int i)
{
    (void)request;
    std::string bad_request_response = 
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 49\r\n"
            "\r\n"
            "<html><body><h1>400 Bad Request</h1></body></html>";

        send(fds[i].fd, bad_request_response.c_str(), bad_request_response.size(), 0);
}

// /*unauthorized*/
// const char *resp_401(std::string request)
// {
// }

// /*forbidden*/
// const char *resp_403(std::string request)
// {
// }

/*not found*/
void Server::resp_404(std::string request, int i)
{
    std::vector<unsigned char> file_content = load_file("./404.html");
    (void)request;
    std::ostringstream oss;
    oss << file_content.size();
    std::string not_found_response = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + oss.str() + "\r\n"
            "\r\n";

        send(fds[i].fd, not_found_response.c_str(), not_found_response.size(), 0);
        send(fds[i].fd, reinterpret_cast<const char*>(file_content.data()), file_content.size(), 0);
}

// /*method not allowe*/
// const char *resp_405(std::string request)
// {
// }

// /*request timeout*/
// const char *resp_408(std::string request)
// {
// }

// /*method not implemented*/
// const char *resp_501(std::string request)
// {
// }

// /*HTTP version not supported*/
// const char *resp_505(std::string request)
// {
// }