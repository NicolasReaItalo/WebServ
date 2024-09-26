/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileInfos.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 10:11:28 by nrea              #+#    #+#             */
/*   Updated: 2024/09/23 15:39:41 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_INFOS_HPP
#define FILE_INFOS_HPP
#include <list>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <exception>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <unistd.h>

std::string getParentDir(std::string uri);

class FileInfos
{
	private:
		struct stat _file;
		bool		_exist;
		bool		_validPath;

	public:
		FileInfos(std::string file_path);
		FileInfos();
		void getInfos(std::string file_path);
		bool file_exist(void);
		bool valid_path(void);
		bool is_regular(void);
		bool is_dir(void);
		bool read_permission(void);
		bool write_permission(void);
		bool exec_permission(void);
		~FileInfos();
};

#endif
