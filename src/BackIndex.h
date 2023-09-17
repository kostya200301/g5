#ifndef BACKINDEX_H
#define BACKINDEX_H
#include <iostream>
#include <vector>
#include <string>
#include <lmdb.h>


using namespace std;

class BackIndex {
public:
	string hash;
	string path;
	string parName;
	MDB_env* env;
	MDB_txn* txn;
	MDB_dbi dbi;
	BackIndex();
	BackIndex(const string& paramName, const string& pathBaket);
	~BackIndex();
	bool Add(const string& word, const string& actionPath, bool com=true);
	bool AddVec(vector<pair<string, string>>& vec);
	//vecot
};


#endif
