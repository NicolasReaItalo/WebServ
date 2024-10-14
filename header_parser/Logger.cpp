/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qgiraux <qgiraux@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 10:17:49 by nrea              #+#    #+#             */
/*   Updated: 2024/10/14 12:34:50 by qgiraux          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

Logger webservLogger(LVL_DEBUG);

std::string Logger::timestamp(void)
{
	time_t now = time(NULL);
	tm* timeinfo = std::localtime(&now);
	char time_stamp[30] = {0};
	std::strftime(time_stamp, sizeof(time_stamp), "[%Y/%m/%d_%H:%M:%S]", timeinfo);
	return std::string(time_stamp);
}

Logger::Logger(int level):_level(level)
{
	_level_colors[LVL_DEBUG] = DEBUG_CLR;
	_level_colors[LVL_INFO] = INFO_CLR;
	_level_colors[LVL_ERROR] = ERROR_CLR;
	_level_str[LVL_DEBUG] = "[DEBUG]";
	_level_str[LVL_INFO] =  "[INFO] ";
	_level_str[LVL_ERROR] = "[ERROR]";

}

Logger::~Logger()
{
}

void	Logger::log(int level, std::ostringstream &oss)
{

	if (level >= _level)
	{
		std::cout<< _level_colors[level]<<" ";
		std::cout<< _level_str[level]<<" ";
		std::cout<< timestamp() << " ";
		std::cout<< oss.str() << " ";
		std::cout<<CLR_RST<<std::endl;
	}
}
