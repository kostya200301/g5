#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>

#include "Baket.h"


uint64_t Baket::GetHash(const uint64_t * data, int len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;
    uint64_t h = seed ^ (len * m);
    const uint64_t * end = data + (len/8);
    while(data != end) {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char * data2 = (const unsigned char*)data;

    switch(len & 7) {
        case 7: h ^= uint64_t(data2[6]) << 48;
        case 6: h ^= uint64_t(data2[5]) << 40;
        case 5: h ^= uint64_t(data2[4]) << 32;
        case 4: h ^= uint64_t(data2[3]) << 24;
        case 3: h ^= uint64_t(data2[2]) << 16;
        case 2: h ^= uint64_t(data2[1]) << 8;
        case 1: h ^= uint64_t(data2[0]);
                h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}


Baket::Baket() {this->start_time = 1; this->end_time = 1;}


Baket::Baket(std::string BID) {
	// Make only read
	std::string db_path = "data/index/" + BID.substr(0, 3) + "/" + BID.substr(3, 3) + "/" + BID;
	this->path = db_path;
	std::string db_path_actions = this->path + "/Actions";
	std::string db_path_info = this->path + "/Info";
	cout << db_path_actions << endl << db_path_info << endl;
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status status = rocksdb::DB::OpenForReadOnly(options, db_path_actions, &db);
        if (!status.ok()) {
		std::cerr << "Unable to open database: " << status.ToString() << std::endl;
	}

	rocksdb::Status status_info = rocksdb::DB::OpenForReadOnly(options, db_path_info, &dbWords);
        if (!status_info.ok()) {
		std::cerr << "Unable to open database: " << status_info.ToString() << std::endl;
	}
}

string Baket::GetBIbyKEYword(const string Kword) {
	rocksdb::ReadOptions read_options;
	string value;
	rocksdb::Status read_status = dbWords->Get(read_options, Kword, &value);

	if (read_status.ok()) {
		return value;
	} else {
		throw "Error read\n";
	}
}



	void Baket::GetUUIDsByWord(std::string word) {
	std::string value;
	rocksdb::Status get_status = db->Get(rocksdb::ReadOptions(), word, &value);
	if (get_status.ok()) {
		std::cout << "Value for key " << word << ": " << value << std::endl;
	} else {
		std::cerr << "Error getting value for key " << word << ": " << get_status.ToString() << std::endl;
	}
}



Baket::Baket(uint64_t curTime, uint64_t timeOut_) {
	this->start_time = curTime;
	this->end_time = curTime + timeOut_;
	this->timeOut = timeOut_;
	this->hash = std::to_string(this->GetHash(&curTime, 8, 0));

	std::string directoryPath = "data/index/" + this->hash.substr(0, 3) + "/" + this->hash.substr(3, 3) + "/" + this->hash;
	this->path = directoryPath;

	// Создаем директорию
	if (!std::filesystem::exists(directoryPath)) {
		if (!std::filesystem::create_directories(directoryPath)) {
//			std::cerr << "Failed to create directory." << std::endl;
			throw "Create dir error";
		}
	}


	// Create rocksDB to save all Actions
	std::string db_path = path + "/Actions";
	std::string db_path_info = path + "/Info";

	rocksdb::Options options;
    options.create_if_missing = true;

	rocksdb::Options options_info;
	options_info.create_if_missing = true;


/*    // Настройки для максимальной производительности записи
    options.write_buffer_size = 256 * 1024 * 1024; // 256 МБ
    options.max_write_buffer_number = 20; // Увеличьте число буферов для аккумулирования операций записи
    options.target_file_size_base = 512 * 1024 * 1024; // 512 МБ
    options.level0_file_num_compaction_trigger = 4; // Больше уровней SST
    options.max_background_compactions = 4; // Многопоточная компакция
    options.max_background_flushes = 2; // Многопоточная фоновая запиc
    options.use_direct_reads = true; // Использование прямых чтений
*/


	rocksdb::Status status_info = rocksdb::DB::Open(options_info, db_path_info, &dbWords);
        if (!status_info.ok()) {
                std::cerr << "Unable to open database: " << status_info.ToString() << std::endl;
        }


	rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
	if (!status.ok()) {
		std::cerr << "Unable to open database: " << status.ToString() << std::endl;
	}


	// DELTE DB!!!
}


bool Baket::AddActionInAllActions(Action& action) {
	rocksdb::WriteOptions write_options;
//	std::cout << "action.uuid, " << action.str << endl;
	rocksdb::Status write_status = db->Put(write_options, action.uuid, action.str);
	if (!write_status.ok()) {
		std::cerr << "Failed to write data: " << write_status.ToString() << std::endl;
	}

}

bool Baket::AddWordInfo(const std::string word, const std::string path_to_BI) {
	this->dbWords->Put(rocksdb::WriteOptions(), word, path_to_BI);
}

using json = nlohmann::json;

bool Baket::AddJSON(const json& json_obj, const string& UUid, const std::string& prefix = "") {
	if (json_obj.is_object()) {
		for (auto it = json_obj.begin(); it != json_obj.end(); ++it) {
			if (it->is_object() || it->is_array()) {
				// Если элемент является объектом или массивом, рекурсивно вызываем функцию
				AddJSON(*it, UUid, prefix + it.key() + ".");
			} else {
				// Если элемент является простым значением, выводим его
				//std::cout << prefix + it.key() << " : " << it.value() << std::endl;
				auto itBI = this->backIndexes.find(prefix + it.key());
	                        if (itBI != this->backIndexes.end()) {
	                                //for (auto word : p.second) {
	                                        itBI->second->Add(to_string(it.value()), UUid, 1);
	                                //}
	                        }
	                        else {
	                                std::shared_ptr<BackIndex> BIPtr = std::make_shared<BackIndex>(prefix + it.key(), this->path);
	                                this->backIndexes.emplace(prefix + it.key(), BIPtr);

                	                AddWordInfo(prefix + it.key(), BIPtr->hash);

                        	        //for (auto word : p.second) {
                        	                this->backIndexes[prefix + it.key()]->Add(to_string(it.value()), UUid, 1);
                        	        //}
                       		 }

            }
        }
    } else if (json_obj.is_array()) {
        for (size_t i = 0; i < json_obj.size(); ++i) {
            if (json_obj[i].is_object() || json_obj[i].is_array()) {
                // Если элемент массива является объектом или массивом, рекурсивно вызываем функцию
                AddJSON(json_obj[i], UUid, prefix +  ".");
            } else {
                // Если элемент массива является простым значением, выводим его
                //std::cout << prefix + "[" + std::to_string(i) + "]" << " : " << json_obj[i] << std::>
		auto itBI = this->backIndexes.find(prefix);
                if (itBI != this->backIndexes.end()) {
                	//for (auto word : p.second) {
                        itBI->second->Add(to_string(json_obj[i]), UUid, 1);
                        //}
                }
                else {
                        std::shared_ptr<BackIndex> BIPtr = std::make_shared<BackIndex>(prefix, this->path);
                        this->backIndexes.emplace(prefix, BIPtr);
                        AddWordInfo(prefix, BIPtr->hash);

                        //for (auto word : p.second) {
             		           this->backIndexes[prefix]->Add(to_string(json_obj[i]), UUid, 1);
                        //}
               }

            }
        }
    }

}

bool Baket::Add(Action& action, bool com) {
	AddActionInAllActions(action);

	AddJSON(action.json, action.uuid);


/*
                AddActionInAllActions(action);
                for (auto p : action.data) {
                        auto it = this->backIndexes.find(p.first);
                        if (it != this->backIndexes.end()) {
                                for (auto word : p.second) {
                                        it->second->Add(word, action.uuid, com);
                                }
                        }
                        else {
				std::shared_ptr<BackIndex> BIPtr = std::make_shared<BackIndex>(p.first, this->path);
                                this->backIndexes.emplace(p.first, BIPtr);

				AddWordInfo(p.first, BIPtr->hash);

                                for (auto word : p.second) {
                                        this->backIndexes[p.first]->Add(word, action.uuid, com);
                                }
                        }
                }
*/

/*
	for (auto p : action.data) {
		auto it = this->backIndexes.find(p.first);
		if (it != this->backIndexes.end()) {
			it->second->Add(it->first, action.GetTimeS());
		}
		else {
			std::shared_ptr<BackIndex> BIPtr = std::make_shared<BackIndex>(p.first, this->path);
			this->backIndexes.emplace(p.first, BIPtr);
//			this->backIndexes.emplace(p.first, new BackIndex(p.first, this->path));
//			this->backIndexes[p.first] = BackIndex(p.first);
			this->backIndexes[p.first]->Add(p.first, action.GetTimeS());
		}
	}*/

/*
	std::string directoryPath = "data/index/" + this->hash.substr(0, 3) + "/" + this->hash.substr(3, 3) + "/" + this->hash;
	std::string filePath = directoryPath + "/example.txt";


	 std::ofstream outfile(filePath, std::ios::app);

	if (outfile.is_open()) {
		outfile << action.GetTimeS() << "\n";
		outfile.close();
	} else {
		std::cerr << "Не удалось открыть файл для записи.\n";
	}
*/
}



bool Baket::Add(std::vector<Action>& actions) {
	std::cout << this->backIndexes.size() << std::endl;
	if (actions.empty()) {return true;}
	for (int i = 0; i < actions.size() - 1; i++) {
		AddActionInAllActions(actions[i]);
		for (auto p : actions[i].data) {
	                auto it = this->backIndexes.find(p.first);
        	        if (it != this->backIndexes.end()) {
				for (auto word : p.second) {
                	        	it->second->Add(word, actions[i].uuid, false);
				}
                	}
                	else {
				std::shared_ptr<BackIndex> BIPtr = std::make_shared<BackIndex>(p.first, this->path);
				this->backIndexes.emplace(p.first, BIPtr);
				for (auto word : p.second) {
	                        	this->backIndexes[p.first]->Add(word, actions[i].uuid, false);
				}
	                }
	        }

	}
	AddActionInAllActions(actions[actions.size() - 1]);
	for (auto p : actions[actions.size() - 1].data) {
                auto it = this->backIndexes.find(p.first);
                    if (it != this->backIndexes.end()) {
			for (auto word : p.second) {
                            it->second->Add(word, actions[actions.size() - 1].uuid, true);
			}
                    }
                    else {
			std::shared_ptr<BackIndex> BIPtr = std::make_shared<BackIndex>(p.first, this->path);
			this->backIndexes.emplace(p.first, BIPtr);
			for (auto word : p.second) {
				this->backIndexes[p.first]->Add(word, actions[actions.size() - 1].uuid, true);
			}
                    }
        }


}


