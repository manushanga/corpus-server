/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef DATALOADER_H
#define DATALOADER_H

#include "corpus.h"

class Cache
{
private:
	Database db;
	DataRaw raw;
public:
    Cache(std::vector<std::string>& files,
		  bool refresh);
	inline Database& getDb() { return db; }; 
	inline DataRaw& getRaw() { return raw; };
	void dumpUniques(const std::string &file);
	void writeFromDatabase(const std::string &file);
	void readToDatabase(const std::string& file);
    virtual ~Cache();
};

#endif
