
#include <filesystem>



#include "BackIndexRock.h"


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





BackIndex::BackIndex() {
	db = NULL;
}


BackIndex::BackIndex(const string& paramName, const string& pathBaket) {

	std::hash<std::string> hasher;
	this->hash = to_string(hasher(paramName));
	// create dir to db
	std::string folderPath = pathBaket + "/BI/" + this->hash.substr(0, 2) + "/" + this->hash.substr(2, 2) + "/" + this->hash;
	this->path = folderPath;
//	std::cout << folderPath << endl;
	if (!fs::exists(folderPath)) { // Проверяем, существует ли уже папка
		if (fs::create_directories(folderPath)) { // Создаем папку и вложенные подпапки (если нужно)
//			std::cout << "Folder created successfully." << std::endl;
		} else {
			std::cerr << "Failed to create folder." << std::endl;
		}
	} else {
		std::cout << "Folder already exists." << std::endl;
	}
	const char* dbPath = folderPath.c_str(); // Путь к базе данных
	// create DB
//	rocksdb::DB* db;
	rocksdb::Options options;
	options.create_if_missing = true;

//	options.use_direct_io_for_flush_and_compaction = true;
//	options.compression = rocksdb::kLZ4Compression; 	// Выбираем тип сжатия (Snappy)
//	options.compression = rocksdb::kSnappyCompression; 	// Выбираем тип сжатия (Snappy)
//	options.compression = rocksdb::kNoCompression;  // off sgatie
//	options.max_background_compactions = 10;
//	options.max_write_buffer_number = 10;
//	options.compaction_style = rocksdb::kCompactionStyleFIFO;
//	options.compaction_style = rocksdb::kCompactionStyleUniversal;

//	options.write_buffer_size = 2 * 64 * 1024 * 1024;
	   options.merge_operator.reset(new MyMergeOperator); // Установка вашего функтора объединения

	options.write_buffer_size = 1024 * 1024 * 1024;
	this->batch = new rocksdb::WriteBatch();


//	options.write_buffer_size = 512 * 1024 * 1024; // 512 МБ
//	options.max_write_buffer_number = 20; // Увеличьте число буферов для аккумулирования операций записи
//	options.target_file_size_base = 1024 * 1024 * 1024; // 1 ГБ
//	options.level0_file_num_compaction_trigger = 10; // Больше уровней SST
//	options.level0_slowdown_writes_trigger = 30; // Увеличьте пороги замедления для уровня L0
//	options.level0_stop_writes_trigger = 40; // Увеличьте пороги остановки записей для уровня L0
//	options.max_background_compactions = 4; // Многопоточная компакция
//	options.max_background_flushes = 2; // Многопоточная фоновая запись
//	options.use_direct_reads = true; // Использование прямых чтений

	rocksdb::Status status = rocksdb::DB::Open(options, dbPath, &db);
	if (!status.ok()) {
		throw "Unable to open database";
	}

}

BackIndex::~BackIndex() {
	cout << "dest: " << db << endl;
	if (db != NULL) {
//		cout << "txn" << txn << endl;
		delete db;
	}
}

void updateOrInsertValue(rocksdb::DB* db, const std::string& key, const std::string& aPath) {
    rocksdb::Status status;

    // Попытка найти значение по ключу
//    std::string existingValue;
//    status = db->Get(rocksdb::ReadOptions(), key, &existingValue);

//    if (status.IsNotFound()) {
        // Значение не найдено, создаем новую запись
//        status = db->Put(rocksdb::WriteOptions(), key, aPath);
//	cout << status.ok() << endl;
//    } else if (status.ok()) {
//        existingValue += "," + aPath;
//        status = db->Put(rocksdb::WriteOptions(), key, existingValue);
//    } else {
        // Обработка ошибки
//        std::cerr << "Error: " << status.ToString() << std::endl;
//    }

	status = db->Merge(rocksdb::WriteOptions(), key, aPath);
//	    if (status.ok()) {
//        std::cout << "Data added successfully." << std::endl;
//    } else {
//        std::cerr << "Error: " << status.ToString() << std::endl;
//    }
}

void updateOrInsertValueGroup(rocksdb::DB* db, const std::string& key, const std::string& aPath, rocksdb::WriteBatch& batch) {
//	std::string existingValue;
	rocksdb::Status status;
//	status = db->Get(rocksdb::ReadOptions(), key, &existingValue);

//	if (status.IsNotFound()) {
		// Значение не найдено, создаем новую запись
//        	batch.Put(key, aPath);
//	} else if (status.ok()) {
//		existingValue += "," + aPath;
//		batch.Put(key, existingValue);
//	} else {
		// Обработка ошибки
//		std::cerr << "Error: "  << std::endl;
//	}

	status = batch.Merge(key, aPath);


}

bool BackIndex::Add(const string& word, const string& actionPath, bool com) {
//	mdb_txn_begin(env, nullptr, 0, &txn);
	// Вызываем функцию для обновления или добавления значения
/*	if (!com) {
		updateOrInsertValueGroup(db, word, actionPath, *(this->batch));
	} else {
		updateOrInsertValueGroup(db, word, actionPath, *(this->batch));
		rocksdb::WriteOptions writeOptions;
		db->Write(writeOptions, this->batch);
		delete this->batch;
		this->batch = new rocksdb::WriteBatch();
	}
*/



	updateOrInsertValue(db, word, actionPath);
	// Завершение транзакции и закрытие окружения
//	mdb_txn_commit(txn);

//	if (com) {
//		mdb_txn_commit(txn);
//		mdb_txn_begin(env, nullptr, 0, &txn);
//	}
}

bool BackIndex::AddVec(vector<pair<string, string>>& vec) {
	rocksdb::WriteBatch batch;
	for (auto el : vec) {
		// Вызываем функцию для обновления или добавления значения
		updateOrInsertValueGroup(db, el.first, el.second, batch);
	}
        // Завершение транзакции и закрытие окружения
	rocksdb::Status status = db->Write(rocksdb::WriteOptions(), &batch);

}

