/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 10:17:49 by nrea              #+#    #+#             */
/*   Updated: 2024/09/19 11:02:53 by nrea             ###   ########.fr       */
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
}

Logger::~Logger()
{
}

void	Logger::log(int level, std::string msg)
{
	if (level >= _level)
	{
		std::cout<< _level_colors[level]<<" ";
		std::cout<< timestamp() << " ";
		std::cout<< msg << " ";
		std::cout<<CLR_RST<<std::endl;
	}
}

void	Logger::log(int level, std::string msg, std::size_t value)
{
	if (level >= _level)
	{
		std::cout<< _level_colors[level]<<" ";
		std::cout<< timestamp() << " ";
		std::cout<< msg << " ";
		std::cout<< value << " ";
		std::cout<<CLR_RST<<std::endl;
	}

}

void	Logger::log(int level, std::string msg1, std::string msg2)
{
	if (level >= _level)
	{
		std::cout<< _level_colors[level]<<" ";
		std::cout<< timestamp() << " ";
		std::cout<< msg1 << " ";
		std::cout<< msg2 << " ";
		std::cout<<CLR_RST<<std::endl;
	}
}
void	Logger::log(int level, std::string msg1, std::string msg2, std::string msg3)
{
	if (level >= _level)
	{
		std::cout<< _level_colors[level]<<" ";
		std::cout<< timestamp() << " ";
		std::cout<< msg1 << " ";
		std::cout<< msg2 << " ";
		std::cout<< msg3 << " ";
		std::cout<<CLR_RST<<std::endl;
	}
}

void	Logger::log(int level, std::string msg1, std::string msg2, std::string msg3,  std::string msg4)
{
	if (level >= _level)
	{
		std::cout<< _level_colors[level]<<" ";
		std::cout<< timestamp() << " ";
		std::cout<< msg1 << " ";
		std::cout<< msg2 << " ";
		std::cout<< msg3 << " ";
		std::cout<< msg4 << " ";
		std::cout<<CLR_RST<<std::endl;
	}
}

// Logger::Logger(int level, std::string file_name):_level(level),_file_name(file_name)
// {

// }
