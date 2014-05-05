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


#ifndef SEARCHER_H
#define SEARCHER_H
#include <algorithm>
#include <vector>
#include <cstring>

#include "corpus.h"
#include "cache.h"

#define BITCOUNT ( 8*sizeof(bits) )

typedef unsigned int uint;
#ifdef __amd64__
typedef unsigned long bits;
#else
typedef unsigned int bits;
#endif

class Searcher
{
private:
    uint uniques,bitsMax;
	std::vector < std::vector<bits> > bitindex;
	std::map < wchar_t, uint > dict;
	std::vector < uint > ranges;
	std::vector< Database::iterator > rdb;
	
	Cache *cache;
	void collect(std::vector< std::wstring >& searchList, 
				 std::vector< std::vector<bits> >& searchWordBits, 
				 std::vector < unsigned >& results );

public:
	enum Status{
		TOO_MANY_STARS, 
		TOO_MANY_QUES, 
		TOO_MANY_WORDS, 
		WRONG_REGEXP,
		LETTER_NOT_IN_ALPHABET,
		OK
	};
	static const int MaxHits=256;
	
	
    Searcher(Cache *c);
	
    Status doSearch(std::wstring searchstring, std::vector< unsigned >& results);
	void getConcordList(std::vector< unsigned >& result, 
						ConcordResults& concordList,
						unsigned rangeL, unsigned rangeR);
    virtual ~Searcher();
};

#endif // SEARCHER_H
