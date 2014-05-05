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

#ifndef LOGGER_H
#define LOGGER_H

#include "filewriter.h"

#define LOG(x) Logger::getSingleton()->write(x);
class Logger
{
private:
	FileWriter *fw;
public:
    Logger() 
	{
		static FileWriter f("corpus.log",FileUtil::UTF16);
		fw = &f;
	};
	static Logger *getSingleton() 
	{
		static Logger log;
		return &log;
	};
	
	void write(std::string s)
	{
		std::fwprintf(fw->ptr(),L"%s\n",s.c_str());
		std::fflush(fw->ptr());
	};
	void write(std::wstring s)
	{
		std::fwprintf(fw->ptr(),L"%ls\n",s.c_str());
		std::fflush(fw->ptr());
	};
	void write(double dbl)
	{
		std::fwprintf(fw->ptr(),L"%lf\n",dbl);
		std::fflush(fw->ptr());
	};
    ~Logger() 
	{
		std::fflush(fw->ptr());
	};
};

#endif // LOGGER_H
