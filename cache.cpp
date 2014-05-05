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

#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>

#include "cache.h"
#include "filereader.h"
#include "filewriter.h"

Cache::Cache(std::vector<std::string>& files, bool refresh)
{
	std::wstring curr;
	unsigned uniq_word_idx=0,word_idx=0;
	if (refresh || !FileUtil::isFileExists("db.cache")) {
		for (size_t i=0;i<files.size();i++) {
		
			FileReader fr( files[i].c_str() ,FileUtil::UTF16);
			fr.getWord(curr);
			raw.reserve(100000000);
			
			while ( !fr.isEOF() ) {
				Position pp;
				
				pp.fid = i;
				pp.pid = word_idx;
				
				Database::iterator t = db.find (curr);
				
				if (t == db.end() ) {
					Word sa;
					sa.id = uniq_word_idx;
					sa.list.push_back(pp);
					db.insert(DatabaseRow(curr,sa));
					uniq_word_idx++;
					raw.push_back(sa.id);
				} else {
					t->second.list.push_back(pp);
					raw.push_back(t->second.id);
				}
				
				fr.getWord(curr);
				word_idx++;
			}
			
			writeFromDatabase("db.cache");
		}
		
	} else {
		readToDatabase("db.cache");
	}
}
void Cache::dumpUniques(const string& file)
{
	FileWriter fw(file.c_str(),FileUtil::UTF16);
	FILE *ff=fw.ptr();
		
	for (Database::iterator it=db.begin();
		it!=db.end();
		it++) {
        std::fwprintf(ff,L"%ls\n", (*it).first.c_str());
	}
}

void Cache::writeFromDatabase( const std::string& file )
{
	FileWriter fw(file.c_str(),FileUtil::UTF16);
	FILE *ff=fw.ptr();
	std::fwprintf(ff,L"%llu\n",db.size());
		
	for (Database::iterator it=db.begin();
		it!=db.end();
		it++) {
        std::fwprintf(ff,L"%u %ls %llu", (*it).second.id, (*it).first.c_str(),(*it).second.list.size());
		for (PositionList::iterator it1=(*it).second.list.begin();
			it1!=(*it).second.list.end();
			it1++) 
		{
            std::fwprintf(ff,L" %u %u",(*it1).fid,(*it1).pid);
		
		}
		std::fwprintf(ff,L"\n");
	}
	std::fwprintf(ff,L"%llu\n",raw.size());
	for (DataRaw::iterator it=raw.begin();
		it!=raw.end();
		it++)
	{
        std::fwprintf(ff,L" %u",*it);
	}
}
void Cache::readToDatabase( const std::string& file )
{
	unsigned size,listsize,id;
	FileReader fr(file.c_str(),FileUtil::UTF16);
	FILE *ff=fr.ptr();
    std::fwscanf(ff,L"%u",&size);
	wchar_t word[256];
	for (unsigned i=0;i<size;i++){
		word[0]=L'\0';
		std::fwscanf(ff,L"%u",&id);

        std::fwscanf(ff,L"%255ls",word);
        std::fwscanf(ff,L"%u",&listsize);

		std::wstring this_word(word);

        DatabaseRow this_data;
		this_data.first=this_word;
		this_data.second.id = id;
        for (unsigned j=0;j<listsize;j++){
			Position pp;
            std::fwscanf(ff,L"%u %u",&pp.fid,&pp.pid);
			this_data.second.list.push_back(pp);
		}

        db.insert(this_data);
	}
    std::fwscanf(ff,L"%u",&size);
    for (unsigned i=0;i<size;i++){
        std::fwscanf(ff,L"%u",&id);
		raw.push_back(id);
    }
	
}

Cache::~Cache()
{

}

