#include <filesystem>
#include <string>
#include <unordered_map>

#include "Searcher.h"


namespace fs = std::filesystem;


class MyMergeOperator : public rocksdb::MergeOperator {
public:
    virtual ~MyMergeOperator() {}
    // Метод для выполнения полного объединения (Full Merge)
    virtual bool FullMerge(const rocksdb::Slice& key,
                           const rocksdb::Slice* existing_value,
                           const std::deque<std::string>& operand_list,
                           std::string* new_value,
                           rocksdb::Logger* logger) const override {
        // operand_list содержит последовательность операций объединения, front() первый операнд
        // Пример: конкатенация всех операндов
        for (const std::string& operand : operand_list) {
            *new_value += operand + " ";
        }
        // Если существующее значение существует, добавьте его к результату
        if (existing_value) {
            *new_value += existing_value->ToString();
        }
        return true;
    }
    // Метод для выполнения частичного объединения (Partial Merge)
    virtual bool PartialMerge(const rocksdb::Slice& key,
                              const rocksdb::Slice& left_operand,
                              const rocksdb::Slice& right_operand,
                              std::string* new_value,
                              rocksdb::Logger* logger) const override {
        // Реализуйте вашу логику частичного объединения данных (Partial Merge) здесь
        // Пример: конкатенация левого и правого операндов
        *new_value = left_operand.ToString() + " " + right_operand.ToString();
        return true;
    }
    // Возвращаем имя оператора объединения
    virtual const char* Name() const override {
        return "MyMergeOperator";
    }
};




void listFiles(const fs::path& dirPath) {
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (fs::is_regular_file(entry)) {
            std::cout << entry.path() << std::endl;
        }
    }
}

Searcher::Searcher(){}

Searcher::Searcher(Request& r){
	cout << "in SerCr\n";
	this->treeBakets = Btree("./testdb", 1);
	this->req = r;
}

map<string, string> Searcher::GetBakets() {
	return this->treeBakets.GetData(this->req.start_time, this->req.end_time);
}

vector<string> GetUUIDtoActionsByRequest() {

}

std::vector<std::string> splitString(const std::string& input) {
    std::vector<std::string> words;
    std::string word;
    for (char c : input) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

std::vector<std::string> GetUIDsByKWordAndWord(const std::string path, const std::string word) {

	rocksdb::Options options;
    options.create_if_missing = true;
        options.merge_operator.reset(new MyMergeOperator);


	cout << "PATH: " << path << endl;
	rocksdb::DB* dbase;
	rocksdb::Status status = rocksdb::DB::Open(options, path, &dbase);
	if (status.ok()) {

		rocksdb::ReadOptions read_options;
		cout << "Start\n";

		cout << "END vivoda\n";
		string value;
		cout << "params: " << word << endl;
		rocksdb::Status read_status = dbase->Get(read_options, "\"" + word + "\"", &value);
		if (read_status.ok()) {
			cout << "DATA: " << value << endl;
			vector<string> vecUUID = splitString(value);
			cout << vecUUID.size() << endl;
			return vecUUID;
		} else {
			cout << "ERROR read: " << read_status.ToString() << endl;
		}
	} else {
		cout << "ERROR OPEN DB\n";
	}
	delete dbase;
	return std::vector<string>();
}


void Searcher::PrintAll(const std::string& dir) {

	vector<vector<string>> VECvecUUID;
	for (auto el : GetBakets()) {
		for (auto pairKeyVal : this->req.keyWords) {
			string keyWord = pairKeyVal.first;
			Baket b = Baket(el.second);
			string id = b.GetBIbyKEYword(keyWord);
			std::cout << "Get sUCSess\n";
			auto vec = GetUIDsByKWordAndWord(b.path + "/BI/" + id.substr(0, 2) + "/" + id.substr(2, 2) + "/" + id, pairKeyVal.second);
			VECvecUUID.push_back(vec);
			cout << el.first << " " << el.second << endl;
		}
	}
	cout << "Peresecheniya:\n";
	std::unordered_map<string, int> mIntersection;
	int lvl = 0;
	for (auto v : VECvecUUID) {
		for (auto el : v) {
			if (mIntersection[el] == lvl) {
				mIntersection[el]++;
			}
		}
		lvl++;
	}
	for (auto els : mIntersection) {
		if (els.second == lvl) {
			cout << els.first << " ";
		}
	}
	cout << endl;


//    fs::path folderPath = dir; // Замените на путь к вашей папке
//    if (fs::is_directory(folderPath)) {
//        listFiles(folderPath);
//    } else {
//        std::cerr << "Invalid folder path." << std::endl;
//    }


}
