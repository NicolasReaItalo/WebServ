/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileInfos.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrea <nrea@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 11:16:07 by nrea              #+#    #+#             */
/*   Updated: 2024/09/23 15:39:57 by nrea             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileInfos.hpp"


void FileInfos::getInfos(std::string file_path)
{
	int ret = stat(file_path.c_str(),  &_file);
	if (ret == 0)
	{
		_validPath = true;
		_exist = true;
		return;
	}
	if (EACCES == errno)
	{
		_exist = false;
		_validPath = false;
	}
	else if (ENOENT == errno)
	{
		//we check that the path eading to it is valid
		FileInfos parent = FileInfos(getParentDir(file_path));
		_exist = false;
		_validPath = parent.valid_path();
	}
}

FileInfos::FileInfos(std::string file_path)
{
	getInfos(file_path);
}


FileInfos::FileInfos():_exist(false)
{
}

FileInfos::~FileInfos()
{
}

bool FileInfos::is_regular(void)
{
	return S_ISREG(_file.st_mode);
}

bool FileInfos::is_dir(void)
{
	return S_ISDIR(_file.st_mode);
}

bool FileInfos::read_permission(void)
{
	return (_file.st_mode & R_OK);
}

bool FileInfos::write_permission(void)
{
	return (_file.st_mode & W_OK);
}

bool FileInfos::exec_permission(void)
{
	return (_file.st_mode & X_OK);
}

bool FileInfos::file_exist(void)
{
	return _exist;
}

bool FileInfos::valid_path(void)
{
	return _validPath;
}
