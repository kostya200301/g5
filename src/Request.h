#ifndef REQUEST_H
#define REQUEST_H

#include <iostream>
#include <vector>
#include <map>

using namespace std;

class Request {
public:
	string start_time;
	string end_time;
	map<string, string> keyWords;
	Request(string start_time_, string end_time_, map<string, string> keyWords_) {
		this->start_time = start_time_;
		this->end_time = end_time_;
		this->keyWords = keyWords_;
	}
	Request(){}

};

#endif
