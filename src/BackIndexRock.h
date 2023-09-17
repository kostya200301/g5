#ifndef BACKINDEX_H
#define BACKINDEX_H
#include <iostream>
#include <vector>
#include <string>
#include <rocksdb/db.h>
#include <rocksdb/merge_operator.h>
#include <rocksdb/slice.h>

using namespace std;

class BackIndex {
public:
	string hash;
	string path;
	string parName;
	rocksdb::DB* db;
	rocksdb::WriteBatch* batch;
	BackIndex();
	BackIndex(const string& paramName, const string& pathBaket);
	~BackIndex();
	bool Add(const string& word, const string& actionPath, bool com=true);
	bool AddVec(vector<pair<string, string>>& vec);
	//vecot
};


#endif
