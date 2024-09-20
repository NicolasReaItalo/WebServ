/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headerParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 13:44:25 by nrea              #+#    #+#             */
/*   Updated: 2024/09/20 13:23:33 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_PARSER_HPP
#define HEADER_PARSER_HPP

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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include "http_errors.hpp"
#include <unistd.h>
#include "Logger.hpp"

#define RST		"\033[0m"
#define RED		"\033[1;31m"
#define GREEN	"\033[1;32m"
#define YELLOW	"\033[1;33m"
#define BLUE	"\033[1;34m"


extern Logger webservLogger;


header_infos handle_get(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes);
// header_infos handle_post(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes);
// header_infos handle_delete(header_infos &response, ServerConfig  & config,int locationIndex,std::map<std::string, std::string> header_attributes);
header_infos response_error(std::string error_code, ServerConfig  & config,int locationIndex);
header_infos response_autoindex(ServerConfig &config, int locationIndex, header_infos response);
bool contains_only_numeric(std::string str);
long getFileSize(std::string filename);
std::string getFileExtension(std::string uri);

bool contains_forbbiden(std::string const &uri);
std::string  convert_uri(std::string const &uri);
std::vector<std::string> splitString(const std::string& str, std::string delimiter);

bool matchContentTypes(std::string file_content_type, std::string accepted_types);

std::string str_todo(int todo);
#endif
