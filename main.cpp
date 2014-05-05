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
    ls.push_back("kk");
    Timer t1;
    Cache cache(ls,false);
	t1.stop();
	std::cout<<"db ok"<<cache.getDb().size()<<std::endl;
	
 
	Searcher ss(&cache);
	
    
    wchar_t *arr[5]={L"a*bb",L"a*b*",L"?*",L"?",L"*"};
    int h=0;

   // while ( h >=0 ){
		std::vector< unsigned > rlist;
		ConcordResults clist;	
		Timer t2;
		std::cout<< ss.doSearch(std::wstring(arr[h]),rlist)<<std::endl;
		ss.getConcordList(rlist,clist,5,5);
		std::cout<<"Time spent:"<<t2.stop()<<std::endl;
		for (size_t i=0;i<clist.size();i++)
		{
            char buffer[1024];
            wchar_to_utf8(clist[i].center.c_str(),clist[i].center.size(),buffer,1023, UTF8_IGNORE_ERROR|UTF8_SKIP_BOM);
			std::cout<<buffer<<std::endl;
			
			
		}
		h--;
    //}
    return 0;
}

