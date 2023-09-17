#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include "Action.h"
#include "QueActions.h"
#include "Baket.h"
#include "BTree.h"
#include "Request.h"
#include "Searcher.h"

#include <rocksdb/db.h>
#include <rocksdb/merge_operator.h>
#include <rocksdb/slice.h>
class MyMergeOperator : public rocksdb::MergeOperator {
public:
    virtual ~MyMergeOperator() {}
    // Метод для выполнения полного объединения (Full Merge)
    virtual bool FullMerge(const rocksdb::Slice& key,
                           const rocksdb::Slice* existing_value,
                           const std::deque<std::string>& operand_list,
                           std::string* new_value,
                           rocksdb::Logger* logger) const override {
        // Реализуйте вашу логику объединения данных (Full Merge) здесь
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



// Генератор случайных чисел
std::mt19937_64 rng(std::random_device{}());

// Функция для генерации случайной строки заданной длины
std::string generateRandomString(size_t length) {
    std::string randomString;
    randomString.reserve(length);
    // Допустимые символы в строке
    const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    // Распределение случайных чисел для выбора символов
    std::uniform_int_distribution<size_t> distribution(0, characters.size() - 1);
    for (size_t i = 0; i < length; ++i) {
        randomString += characters[distribution(rng)];
    }
    return randomString;
}




void MakeQueue(QueActions<Action>& que) {
	std::ifstream inputFile("topic.txt"); // Замените "example.txt" на путь к вашему файлу

    if (!inputFile.is_open()) {
        std::cerr << "Не удалось открыть файл!" << std::endl;
        return;
    }

    std::string line;
	int i = 0;
    while (std::getline(inputFile, line)) {
	Action act = Action(line);
	que.Add(act);
	i++;
    }

    inputFile.close();

}

void printRocksDB(const std::string& db_path) {
	MyMergeOperator merge_operator;

    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
	options.merge_operator.reset(new MyMergeOperator);
    rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);

    if (!status.ok()) {
        std::cerr << "Ошибка при открытии базы данных: " << status.ToString() << std::endl;
        return;
    }

    rocksdb::ReadOptions read_options;
    rocksdb::Iterator* it = db->NewIterator(read_options);

    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        std::string value = it->value().ToString();
        std::cout << "Ключ: " << key << ", Значение: " << value << std::endl;
    }

    if (!it->status().ok()) {
        std::cerr << "Ошибка при обходе базы данных: " << it->status().ToString() << std::endl;
    }
	std::cout << "END\n";
    delete it;
    delete db;
}


using namespace std::chrono;


void TestSearch() {
	map<string, string> w = {{"data.body.Event.System.Task", "10"}, {"data.action", "execute"}};
//      map<string, string> w = {{"data.action", "execute"}};
        Request r = Request("1694532139241", "1694732139241", w);
        Searcher s = Searcher(r);
        s.PrintAll("lol");

}



void TestRocksDbSpeed() {
	rocksdb::DB* db;
	        rocksdb::Options options;
        options.create_if_missing = true;
           options.merge_operator.reset(new MyMergeOperator); // Установка вашего функтора объединения
        rocksdb::Status status = rocksdb::DB::Open(options, "./test1", &db);
        if (!status.ok()) {
                throw "Unable to open database";
        }
	auto s1 = chrono::steady_clock::now();
	for (int i = 0; i < 1000000; i++) {
		if (i % 1000 == 0) {
			auto s2 = chrono::steady_clock::now();
			std::cout << i << ") " << chrono::duration_cast<chrono::milliseconds>(s2 - s1).count() << "ms" << std::endl;
			s1 = chrono::steady_clock::now();
		}
		db->Merge(rocksdb::WriteOptions(), "lol", generateRandomString(6) + "gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc6666666666666666666666666666666666666666666gny6rfki8ynf56khfyfhhhbbccvkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkgctyicykccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc66666666666666666666666666666666666666666666666666666666666ggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggtfthvhvhtcthhvbjfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhwgny6rfki8ynf56khfyfhhhbbcfdhfdhuuudbwejwendwnedhw");
	}
	delete db;

}




int main() {
//	cout << "start\n";
//	for (int i = 0; i < 10; i++) {
//		auto start1 = chrono::steady_clock::now();
//		TestRocksDbSpeed();
//		auto start2 = chrono::steady_clock::now();
//		std::cout << i << ") " << chrono::duration_cast<chrono::milliseconds>(start2 - start1).count() << "ms" << std::endl;
//	}
//	exit(1);

//	uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//	std::cout << ms << std::endl;
//	exit(0);


//	map<string, string> w = {{"data.body.Event.System.Task", "10"}, {"data.action", "execute"}};
//	map<string, string> w = {{"data.action", "execute"}};
//	Request r = Request("1694532139241", "1694732139241", w);
//	Searcher s = Searcher(r);
//	s.PrintAll("lol");


	Btree backets = Btree("./testdb", 0);
	//Ключ: data.body.Event.System.Task, Значение: 6613064059438764800
	//Ключ: data.category.low, Значение: data/index/702/231/7022314576534891456
	//Ключ: data.object.name, Значение: data/index/702/231/7022314576534891456
	//Ключ: data.status, Значение: 1822355192588271656
//	printRocksDB("data/index/182/564/18256464624817333207/BI/66/13/6613064059438764800");
//	printRocksDB("./data/index/182/564/18256464624817333207/BI/75/05/7505263013960753053");
//	printRocksDB("./data/index/182/564/18256464624817333207/Info");
//	printRocksDB("./testdb");
	QueActions<Action> queue;
	MakeQueue(queue);
	 auto currentTime = std::chrono::system_clock::now();
	    uint64_t currentTimeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime.time_since_epoch()).count();

	Baket b(currentTimeInSeconds, 2500000);
	backets.AddData(to_string(b.start_time), b.hash, 1);
	cout << "Stop\n";
//	exit(0);
	for (int i = 0; i < 10; i++) {
	int u = 0;
	auto start = chrono::steady_clock::now();
	while (queue.GetSize() != 0) {
		if (u % 10000 == 0) {
			auto end = chrono::steady_clock::now();
			std::cout << u << ") " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << std::endl;
			start = chrono::steady_clock::now();
		}
		Action a = queue.Get();
		b.Add(a, 1);
		u++;
	}
		MakeQueue(queue);

	}
}