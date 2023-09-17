#ifndef ACTION_H
#define ACTION_H

#include <iostream>
#include <string>
#include <chrono>
#include <map>
#include <vector>
//#include <nlohmann/json.hpp>
#include <random>
#include "UUID.h"
#include "NER.h"

using namespace std;
using namespace std::chrono;

std::string generateRandomString(size_t length);



class Action {
private:
	string time;
	uint64_t timeInt;

public:
	map<string, vector<string>> data;
	string str;
	string uuid;
	nlohmann::json json;
public:
	Action() {
		auto tm = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
		this->time = to_string(tm);
		this->timeInt = tm;
		this->uuid = UUID::GetUUID();
	}

//	~Action() {
//		cout << "DESTRUCKTOR: " << time << endl;
//	}

	Action(std::string json_) {
		this->str = json_;
		NER nerser = NER(json_);
		this->json = nerser.GetJSON();
		this->uuid = UUID::GetUUID();
		auto tm = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
                this->time = to_string(tm);
                this->timeInt = tm;

	}


	void GetRand() {
		vector<string> names = {"message", "info", "graciec", "fignya", "ideya", "polychit", "netInfoCom"};
		for (auto n : names) {
			vector<string> v1;
			for (int i = 0; i < 5; i++) {
				v1.push_back(generateRandomString(3));
			}
			data[n] = v1;
			for (auto p : v1) { this->str += p + " ";}
		}
	}

	uint64_t GetTime() {
		return this->timeInt;
	}

	string GetTimeS() {
		return this->time;
	}
};


#endif
