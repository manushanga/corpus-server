#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <locale>
#include <stdexcept>

#include "corpus.h"
#include "cache.h"
#include "searcher.h"
#include "filereader.h"
#include "filewriter.h"
#include "fileutil.h"
#include "timer.h"
#include "utf8.h"

using namespace std;

int main ( int argc, const char **argv )
{
	std::vector< std::string > ls;
	for (int i=1;i<argc;i++){
		ls.push_back(std::string(argv[i]));
	}
	Timer t1;
	Cache cache(ls,false);
	t1.stop();
	std::cout<<"db ok"<<cache.getDb().size()<<std::endl;
	

	Searcher ss(&cache);
	
	
	wchar_t *arr[5]={L"their nativity",L"a*b*",L"?*",L"?",L"*"};
	int h=4;

	while ( h >=0 ){
		std::vector< unsigned > rlist;
		ConcordResults clist;	
		Timer t2;
		std::cout<<arr[h]<<std::endl;
		Searcher::Status st= ss.doSearch(std::wstring(arr[h]),rlist);
		if (st == Searcher::OK){
			ss.getConcordList(rlist,clist,5,5);
			std::cout<<"Time spent:"<<t2.stop()<<std::endl;
			char buffer[128];
			char buffer_right[128];
			char buffer_left[128];
			
			for (size_t i=0;i<clist.size();i++)
			{
				buffer[0]='\0';
				buffer_left[0]='\0';
				buffer_right[0]='\0';
				
				wchar_to_utf8(clist[i].left.c_str(),clist[i].left.size(),buffer_left,127, UTF8_IGNORE_ERROR|UTF8_SKIP_BOM);
				wchar_to_utf8(clist[i].right.c_str(),clist[i].right.size(),buffer_right,127, UTF8_IGNORE_ERROR|UTF8_SKIP_BOM);
				wchar_to_utf8(clist[i].center.c_str(),clist[i].center.size(),buffer,127, UTF8_IGNORE_ERROR|UTF8_SKIP_BOM);
				std::cout<<buffer_left<<"|"<<buffer<<"|"<<buffer_right<<std::endl;
				
				
			}
		} else {
			std::cout<<"query failed"<<std::endl;
		}
		h--;
	}
	return 0;
}

