/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 11:26:02 by qgiraux           #+#    #+#             */
/*   Updated: 2024/09/02 13:47:05 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <cstring>
#include <map>
#include <poll.h>
#include <cstdio>
#include <csignal>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1000

std::string get_request_path(const std::string &request);
std::string get_mime_type(const std::string &path);
std::vector<unsigned char> load_file(const std::string &filename);
std::string get_method_type(const std::string &request);


#endif
