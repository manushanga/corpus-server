#ifndef CORPUS_H
#define CORPUS_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include "utils.h"

using namespace std;

struct ConcordResultRow {
	wstring left;
	wstring center; 
	wstring right;
};

struct Position {
	unsigned fid; // file id
	unsigned pid; // position id in file
};

typedef vector< Position > PositionList;

struct Word {
	PositionList list;
	unsigned id;
};

typedef pair< wstring, Word > DatabaseRow;
typedef map< wstring, Word > Database;
typedef vector< unsigned > DataRaw;
typedef vector< ConcordResultRow > ConcordResults;

#endif