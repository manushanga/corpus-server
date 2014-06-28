#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <cctype>
#include <locale>
#include <stdexcept>
#include <functional> 
#include <fcgio.h>

#include "corpus.h"
#include "cache.h"
#include "searcher.h"
#include "filereader.h"
#include "filewriter.h"
#include "fileutil.h"
#include "timer.h"
#include "logger.h"
#include "utf8.h"
#include "regexp.h"

using namespace std;

// Maximum bytes
const unsigned long STDIN_MAX = 1000000;
// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}
string get_request_content(const FCGX_Request & request) {
    char * content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
    unsigned long content_length = STDIN_MAX;

    if (content_length_str) {
        content_length = strtol(content_length_str, &content_length_str, 10);
        if (*content_length_str) {
            cerr << "Can't Parse 'CONTENT_LENGTH='"
                 << FCGX_GetParam("CONTENT_LENGTH", request.envp)
                 << "'. Consuming stdin up to " << STDIN_MAX << endl;
        }

        if (content_length > STDIN_MAX) {
            content_length = STDIN_MAX;
        }
    } else {
        // Do not read from stdin if CONTENT_LENGTH is missing
        content_length = 0;
    }

    char content_buffer[content_length];
    cin.read(content_buffer, content_length);
    content_length = cin.gcount();

    // Chew up any remaining stdin - this shouldn't be necessary
    // but is because mod_fastcgi doesn't handle it correctly.

    // ignore() doesn't set the eof bit in some versions of glibc++
    // so use gcount() instead of eof()...
    do cin.ignore(1024); while (cin.gcount() == 1024);

    string content(content_buffer, content_length);

    return content;
}

int main(int argc, char *argv[]) {
    char instance='\0';
    vector< std::string > ls;

    if (argc<2){   
        cout<<"Invalid arguments! try corpus <character> <data_file1> [data_file2] ... [data_fileN]"<<endl;
        exit(0);
    }
    instance = argv[1][0];
    for (int i=2;i<argc;i++){
        ls.push_back(std::string(argv[i]));
    }
    cout<<"Note: corpus-server only accepts UTF-16 data files, please convert"<<endl
        <<"      if they are not in the correct format. No warnings will be given."<<endl;
    cout<<"Note: If you changed your data, please delete db.cache manually."<<endl;
	cout<<"loading cache..."<<endl;
    Timer t1;
    Cache cache(ls,false);
	//cache.dumpUniques("uniq.txt");
	t1.stop();
	cout<<"cache loaded. db size: "<<cache.getDb().size()<<endl;
	cout<<"preprocessing data..."<<endl;
	Searcher ss(&cache);
	cout<<"done."<<endl<<endl;
	
	
	std::vector< unsigned > rlist;
	ConcordResults clist;	

    // Backup the stdio streambufs
    streambuf * cin_streambuf  = cin.rdbuf();
    streambuf * cout_streambuf = cout.rdbuf();
    streambuf * cerr_streambuf = cerr.rdbuf();

    FCGX_Request request;
    cout<<"ready."<<endl;

    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0) {
        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);

        const char * uri = FCGX_GetParam("REQUEST_URI", request.envp);
		string content = get_request_content(request);
		// strip from end, FF doesn't append \n but chrome does
		//content = trim(content);
		//LOG(content)

		
		if (content.length() > 0 && content.length() < 128) {
			std::string query;
			int lc,lr;
			cout << "Content-type: text/html\r\n"
					<< "\r\n"
					<< "<html>\n"
					<< "  <head>\n"
					<< "   <meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
					<< "    <title>Results</title>\n"
					<< "  </head>\n"
					<< "  <body>\n"
					<< "    <script src=\"FileSaver.js\"></script> \n"
					<< "    <script>\n"
					<< "    function save(){ \n"
					<< "      var blob = new Blob([document.getElementById(\"results\").textContent], {type: \"text/plain;charset=utf-8\"}); \n"
					<< "      saveAs(blob, \"results.txt\"); \n"
					<< "    }\n"
					<< "    function back() { window.history.back(); } \n"
					<< "    </script>\n"
					<< "    <button onclick=\"back()\">Home</button>\n"
					<< "    <button onclick=\"save()\">Download</button>\n";

			int ret=RegExp::validate_post(content.c_str(),query,lc,lr,instance);	
			query = trim(query);
			if (ret && lr>=0 && lc>=0 && lr<6 && lc<6 && query.size() >0 ) {
				wchar_t wbuffer[128];
				utf8_to_wchar(query.c_str(),query.size(),wbuffer,127,UTF8_IGNORE_ERROR|UTF8_SKIP_BOM);
				wstring wquery(wbuffer);
				LOG(L"---");
				
				LOG(wquery)
				LOG(L"---");
				rlist.clear();
				clist.clear();

				/*
				cout<<"<p>"<<query<<"</p>\n";
				cout<<"<p>"<<lc<<"</p>\n";
				cout<<"<p>"<<lr<<"</p>\n";
				*/
				Timer t1;
				Searcher::Status st = ss.doSearch(wquery,rlist);
				double t1end=t1.stop();
					
				if (st == Searcher::OK){
					Timer t2;
					ss.getConcordList(rlist,clist,(unsigned)lc,(unsigned)lr);
					double t2end=t2.stop();
					cout<<"<center><table id=\"results\">";
					LOG(t1end);
					char buffer[1024];
					for (ConcordResults::iterator it=clist.begin();
						it!=clist.end();
						it++)
					{
						cout<<"<tr>";
						buffer[0]=L'\0';
						wchar_to_utf8(it->left.c_str(),it->left.size(),buffer,1023, UTF8_IGNORE_ERROR|UTF8_SKIP_BOM);
						cout<<"<td style=\"text-align: right; white-space: nowrap;\">"<<buffer<<"</td>\n";
						buffer[0]=L'\0';
						wchar_to_utf8(it->center.c_str(),it->center.size(),buffer,1023, UTF8_IGNORE_ERROR|UTF8_SKIP_BOM);
						cout<<"<td style=\"text-align: center; color: red; white-space: nowrap; \">"<<buffer<<"</td>\n";
						buffer[0]=L'\0';
						wchar_to_utf8(it->right.c_str(),it->right.size(),buffer,1023, UTF8_IGNORE_ERROR|UTF8_SKIP_BOM);
						cout<<"<td style=\"white-space: nowrap;\">"<<buffer<<"</td>\n";
						cout<<"</tr>";
						
					}
					cout<<"</table><div style=\"font-size:8pt;\"><br/><br/>Search : "<<t1end<<"s.<br/>"
						<<"Assembling : "<<t2end<<"s.<br/>" 
						<<"</div></center>\n";
				} else if (st == Searcher::LETTER_NOT_IN_ALPHABET) {
					cout<<"<p>Not in alphabet, try again.</p>";
				} else if (st == Searcher::TOO_MANY_WORDS) {
					cout<<"<p>Too many words, try again.</p>";
				} else if (st == Searcher::TOO_MANY_QUES) {
					cout<<"<p>Too many '?'s, try again.</p>";
				} else if (st == Searcher::TOO_MANY_STARS) {
					cout<<"<p>Too many '*'s, try again.</p>";
				} else if (st == Searcher::WRONG_REGEXP) {
					cout<<"<p>Wrong Regular Expression, try again.</p>";
				} else {
					cout<<"<p>Generic error.</p>";
				}
				
				
			} else {
				cout <<"Invalid post request.";
			}
			
		} else {
			cout <<"Can't understand query.";
		}
		cout<<"</body></html>\n";
		
		
        // Note: the fcgi_streambuf destructor will auto flush
    }

    // restore stdio streambufs
    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);

    return 0;
}
