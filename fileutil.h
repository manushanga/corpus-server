/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2013  Madura Anushanga <madura.x86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <cstdio>
#include <iostream>

class FileUtil
{
public:
	enum FILEMODE{UTF8,UTF16,UTF16BE,UTF16LE};
	static bool isFileExists(const std::string filename)
	{
		FILE *file = std::fopen(filename.c_str(),"r");
		if (file) {
			std::fclose(file);
			return 1;
		} else {
			return 0;
		}
	}
	static const char *getFilemode(FILEMODE mode)
	{
#if defined(__linux__)
		switch (mode)
		{
			case UTF8:
				return "UTF8";
			case UTF16:
				return "UTF16";
			case UTF16BE:
				return "UTF16BE";
			case UTF16LE:
				return "UTF16LE";
			default:
				return "UTF8";
		}
#elif defined(_WIN32)
		switch (mode)
		{
			case UTF8:
				return "UTF-8";
			case UTF16:
				return "UTF-16";
			case UTF16BE:
				return "UNICODE";
			case UTF16LE:
				return "UTF-16LE";
			default:
				return "UTF-8";
		}
#endif
	}
};

#endif // FILEUTIL_H
