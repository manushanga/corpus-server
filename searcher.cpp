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

#include <iostream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <vector>

#include "searcher.h"
#include "filewriter.h"
#include "logger.h"
#include "regexp.h"
namespace std{
template<typename FROM>
std::wstring to_wstring(FROM f){
	std::wstringstream ss;
	ss<<f;
	return ss.str();
}
template<typename FROM>
std::string to_string(FROM f){
	std::stringstream ss;
	ss<<f;
	return ss.str();
}	
};

void ReplaceStringInPlace(std::wstring& subject, const std::wstring& search,
						const std::wstring& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::wstring::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

inline void setBit(bits *b,int n)
{
	*b=*b|1<<( n );
}

inline int getBit(const std::vector< bits >& b, int n)
{
	return (b[n / BITCOUNT] >> ( BITCOUNT - (n % BITCOUNT) -1)) & 0x00000001;
}
inline int getBit(bits b, int n)
{
	return ( b>>(BITCOUNT - n - 1) ) & 0x00000001;
}
inline int maskedCompare(bits a, bits mask)
{
	return (a & mask) == mask;
}
inline void setBit(std::vector <bits> &b, int n)
{
	setBit(&b[n / BITCOUNT], n % BITCOUNT );
}
void printBits(vector< bits >& vec)
{
	cout<<"<p>";
    for (int j=0;j<vec.size();j++) {
		for (int i=0;i<BITCOUNT;i++)
		{
			unsigned int x=vec[j]>>(BITCOUNT-i -1) & 0x00000001;
			cout<<x;
		}
    }
    cout<<"</p>"<<endl;

}
int getFirstSetBit(std::vector <bits> &b)
{
	for (int i=0;i<(int)b.size();i++){
		if (b[i] != 0) {
			for (int k=0;k<BITCOUNT;k++){
				if (getBit(b[i],k) == 1){
					return i*BITCOUNT+k;
				}
			}
		}
	}
}
bool compareBitsVectors(const vector< bits >& a, const vector< bits >& b)
{
	for (int i=0;i<a.size()*BITCOUNT;i++){
		int xa= getBit(a,i);
		int xb= getBit(b,i);
		if (xa > xb) {
			return false;
		} else if (xb<xa){
			return true;
		}
	}
	return false;
}

inline bool maskedCompare(std::vector<bits>&a, std::vector<bits>&mask)
{
	bool allMasksMatch=true;
	for (uint i=0;i<(uint)a.size();i++) {
		if (!maskedCompare(a[i],mask[i])){
			//printBits(bitindex[j]);
			
			allMasksMatch=false;
			break;
		}
	}
	return allMasksMatch;
}
Searcher::Searcher(Cache *c)
{
	cache = c;
    rdb.resize(c->getDb().size());
	uint x=0;
   
	for (Database::iterator it=c->getDb().begin();it!=c->getDb().end();it++)
	{
		rdb[it->second.id] = it;
		for (int j=0;j<(int)it->first.size();j++) {
            if (dict.find(it->first.c_str()[j]) == dict.end()){
                dict.insert(pair<wchar_t, uint>(it->first.c_str()[j],x));
				x++;
            }
        }
	}
	
    uniques = (uint)dict.size();
	if (uniques > 512) {
		LOG(std::wstring(L"WARNING: Unique character count is higher than 512, check your data! Performance will get slowed down."));
	}
	bitsMax=uniques/BITCOUNT + 1;
    bitindex.reserve(c->getRaw().size());

    for (std::vector<Database::iterator>::iterator it=rdb.begin(); it!=rdb.end(); it++  )
	{
        vector<bits> vbits;
        vbits.resize(bitsMax);
		for (int i=0;i<(*it)->first.size();i++) {
			setBit(vbits,dict[(*it)->first[i]]);
		}
        bitindex.push_back(vbits);
    }
    
    /*std::sort(bitindex.begin(),bitindex.end(),compareBitsVectors);

    ranges.resize(bitsMax*BITCOUNT);
    for (int i=0;i<(int)ranges.size();i++){
        ranges[i] = (uint)-1;
    }
    
    int firstSetBit=-1;
    for (int i=0;i<(int)bitindex.size();i++){
        //print(bitindex[i]);
        int newFirstSetBit=getFirstSetBit(bitindex[i]);

        if (firstSetBit != newFirstSetBit){
            ranges[newFirstSetBit] = i;
            firstSetBit = newFirstSetBit;
        }

    }
*/
}

void Searcher::collect(std::vector< std::wstring >& searchList,
					   std::vector < std::vector<bits> >& searchWordBits,
					   std::vector < unsigned >& results )
{
	if (!searchList.size())
		return;
	
	wchar_t *text,*regexp,*lastText,*lastRegexp;
	regexp=(wchar_t *)searchList[0].c_str();
	bool allMasksMatch;
/*
    for (uint j=0;j<(uint)bitindex.size();j++) {
        for (int i=0;i<bitsMax;i++) {

            if (maskedCompare(bitindex[j][i],searchFirstWordBits[i])){
               // print(*it);
            }
        }
    }*/
					
	if (searchList.size() == 1) {
		for (uint j=0;j<(uint)bitindex.size();j++) {
			
			if (maskedCompare(bitindex[j],searchWordBits[0])){
				text=(wchar_t *)rdb[j]->first.c_str();
				if (RegExp::match(regexp,text))
				{
					for (PositionList::iterator it1=rdb[j]->second.list.begin();
							it1!=rdb[j]->second.list.end();
							it1++)
							
					{
						results.push_back(it1->pid);
						if (results.size() > MaxHits)
						{
							return;
						}
					}
				}
			}
		}
		
	} else {
		for (uint j=0;j<(uint)bitindex.size();j++) {

			if (maskedCompare(bitindex[j],searchWordBits[0])){
				text=(wchar_t *)rdb[j]->first.c_str();
				if (RegExp::match(regexp,text))
				{
					
					for (PositionList::iterator it1=rdb[j]->second.list.begin();
							it1!=rdb[j]->second.list.end();
							it1++)
							
					{
						bool found=true;
						for (uint pos=it1->pid+searchList.size()-1,sidx=searchList.size()-1; pos>it1->pid; pos--,sidx--) 
						{
							lastText=(wchar_t *) rdb[ cache->getRaw()[pos] ]->first.c_str();
							lastRegexp=(wchar_t *)searchList[sidx].c_str();
							bool mm = maskedCompare(bitindex[ cache->getRaw()[pos] ], searchWordBits[sidx]);
							if (!mm || (mm && !RegExp::match(lastRegexp,lastText)) ){
								found=false;
								break;
							}
						}
						if (found)
						{
							results.push_back(it1->pid);
							if (results.size() > MaxHits)
							{
								return;
							}
						}
					}
				}
			}
			
		}
	}

}
Searcher::Status Searcher::doSearch(std::wstring searchstring,
						  std::vector< unsigned >& results)
{
	wchar_t *this_regex, *this_text;
	bool first=true;
	std::vector<std::wstring>::iterator itstr;
	std::vector<std::wstring> search;
	std::wstringstream ss(searchstring);
	std::wstring str;
	int cstr_wr;
	wchar_t cstr[256];
	std::vector < std::vector<bits> > sbits;
	size_t start_pos,from;
	
	while (ss.good()){
    ss>>str;
		start_pos=0;
		from=0;
		cstr_wr=0;
		
		for (size_t i=0;i<str.size();i++){
			if (i<str.size()-1 && str[i]==L'\\' && (str[i+1]==L'*' ||
								str[i+1]==L'?')) {
				cstr[cstr_wr++]=L'\\';
				cstr[cstr_wr++]=str[i+1];
				if (dict.find(str[i+1]) == dict.end()){
					return Searcher::LETTER_NOT_IN_ALPHABET;
				}

			} else if (str[i]==L'*') {
				cstr[cstr_wr++]=L'.';
				cstr[cstr_wr++]=L'*';
			} else if (str[i]==L'?') {
				cstr[cstr_wr++]=L'.';
			} else {
				cstr[cstr_wr++]=str[i];
				if (dict.find(str[i]) == dict.end()){
					return Searcher::LETTER_NOT_IN_ALPHABET;
				}
			}
		}
		cstr[cstr_wr++]=L'\0';
		std::wstring wstr(cstr);
		if (RegExp::check((wchar_t*)wstr.c_str())) {
			search.push_back(wstr);
		} else {
			return Searcher::WRONG_REGEXP;
		}
	}
	bool start=true;
	bool end=true;
	int stars=0,queses=0;
	
	while (search.size()>0 && search[0]==L".*")
		search.erase(search.begin());
	while (search.size()>0 && search[search.size()-1]==L".*")
		search.erase(search.end());
	
	if (search.size() == 0){
		return Searcher::WRONG_REGEXP;
	}
	
	for (size_t i=0;i<search.size();i++) {
	
		if (search[i].find(L'*') != wstring::npos) {
			stars++;
		}
		if (search[i].find(L".*") != wstring::npos) {
			queses++;
		}
	}
	if (stars > 4 ) {
		return Searcher::TOO_MANY_STARS;
	} else if (queses > 4) {
		return Searcher::TOO_MANY_QUES;
	} 
	
	sbits.resize(search.size());
	for (uint j=0;j<(uint)search.size();j++){
		sbits[j].resize(bitsMax);
		
		for (uint i=0;i<(uint)sbits[j].size();i++){
			sbits[j][i]=0;
		}
		
		for (size_t i=0;i<search[j].size();i++){
			if (i<search[j].size()-1 
				&& search[j][i]==L'\\' 
				&& (search[j][i+1]==L'*' || search[j][i+1]==L'?' || search[j][i+1]==L'.')) {
				setBit(sbits[j],dict[search[j][i+1]]);
			} else if (search[j][i]!=L'*' && search[j][i]!=L'?' && search[j][i]!=L'.') {
				setBit(sbits[j],dict[search[j][i]]);
			}
		}
	}
	
	
	if (search.size() > 8){
		return Searcher::TOO_MANY_WORDS;
	}
	std::cout<<"size:"<<search.size()<<std::endl;
	//LOG(std::wstring(L"SEARCH: ")+std::to_wstring<int>((int) search.size()) );
	collect(search,sbits,results);
	return Searcher::OK;
}

void Searcher::getConcordList(std::vector< unsigned >& result, 
							ConcordResults& concordList,
							unsigned rangeL, unsigned rangeR)
{
	for (std::vector< unsigned >::iterator it=result.begin(); it!=result.end(); it++) 
	{
		unsigned startPos;
		if (rangeL < *it) {
			startPos =  *it - rangeL;
		} else {
			startPos =0;
		}
		unsigned endPos;
		if ( *it + rangeR < cache->getRaw().size() ) {
			endPos =  *it + rangeR + 1;
		} else {
			endPos = (unsigned)cache->getRaw().size();
		}
		ConcordResultRow row;
		unsigned x;
		if (rangeL >0) {
			for (x=startPos;x< *it;x++) {
				row.left += rdb[ cache->getRaw()[x] ]->first + L" ";
			}
		}
		row.center = rdb[  cache->getRaw()[*it] ]->first;
		if (rangeR >0) {
			x=*it;
			for (x++;x<=endPos;x++){
				row.right += rdb[ cache->getRaw()[x] ]->first + L" ";
			}
			
		}
		concordList.push_back(row);
	}
}

Searcher::~Searcher()
{

}

