#ifndef BAKET_H
#define BAKET_H

#include <chrono>
#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <memory>
#include <rocksdb/db.h>
#include <string>
#include "Action.h"
#include "BackIndexRock.h"
#include <vector>
using json = nlohmann::json;



class Baket {
public:
	uint64_t start_time; // Included
	uint64_t end_time; // Included
	std::string hash; // The identifier
	string path;
	rocksdb::DB* db; // DB to save all actions
	rocksdb::DB* dbWords; // DB to save info about backIndexes
	uint64_t timeOut;
	unordered_map<string, std::shared_ptr<BackIndex> > backIndexes;
private:
	bool AddWordInfo(const std::string word, const std::string hash);
	uint64_t GetHash(const uint64_t * data, int len, uint64_t seed);
	bool AddActionInAllActions(Action& action);
	bool AddJSON(const json& json_obj, const string& UUid, const std::string& prefix);
public:
	Baket();
	Baket(std::string hash);
	Baket(uint64_t curTime, uint64_t timeOut_);
	bool Add(Action& action, bool com);
	bool Add(std::vector<Action>& actions);
	std::string GetBIbyKEYword(const std::string Kword);
	void GetUUIDsByWord(std::string word);
};


#endif
