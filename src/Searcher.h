#ifndef SEARCHER_H
#define SEARCHER_H

#include <iostream>
#include <vector>
#include "Request.h"
#include "BTree.h"
#include "Baket.h"

class Searcher {
private:
//	vector<string> GetBakets()
	Request req;
	map<string, string> GetBakets();
	Btree treeBakets;
public:
	Searcher();
	Searcher(Request& r);
	void PrintAll(const std::string& dir);

};


#endif
