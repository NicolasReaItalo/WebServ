/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 09:30:59 by nrea              #+#    #+#             */
/*   Updated: 2024/09/19 11:02:17 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <ctime>

#define LVL_DEBUG 0
#define LVL_INFO 1
#define LVL_ERROR 2

#define CLR_RST		"\033[0m"
#define DEBUG_CLR	"\e[0;37m"
#define INFO_CLR	"\e[0;34m"
#define ERROR_CLR	"\e[0;31m"


class Logger
{
private:
	int				_level;
	std::map<int, std::string> _level_colors;
	std::string timestamp(void);
	// std::ofstream	_out_file;
	// std::string		_file_name;

public:
	// Logger(int level, std::string file_name);
	Logger(int level);
	~Logger();
	void	log(int level, std::string msg);
	void	log(int level, std::string msg, std::size_t value);
	void	log(int level, std::string msg1, std::string msg2);
	void	log(int level, std::string msg1, std::string msg2, std::string msg3);
	void	log(int level, std::string msg1, std::string msg2, std::string msg3, std::string msg4);
};

#endif
