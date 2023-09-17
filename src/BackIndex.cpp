
#include <filesystem>

#include "BackIndex.h"

namespace fs = std::filesystem;


BackIndex::BackIndex() {
	this->env = NULL;
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
	// create DB
	const char* dbPath = folderPath.c_str(); // Путь к базе данных LMDB
	// Создание LMDB окружения
	mdb_env_create(&env);
	// Устанавливаем размер базы данных (1 ГБ)
	size_t mapSize = 1ULL * 1024ULL * 1024ULL * 1024ULL; // 1 GB
//	size_t mapSize = 1ULL * 1024ULL * 1024ULL * 1024ULL; // 1 GB
	mdb_env_set_mapsize(env, mapSize);

	mdb_env_set_maxdbs(env, 1); // Устанавливаем максимальное количество баз данных
	mdb_env_open(env, dbPath, MDB_CREATE | MDB_WRITEMAP, 0664);

	// Открытие LMDB транзакции и базы данных
//	MDB_txn* txn;
	mdb_txn_begin(env, nullptr, 0, &txn);
	mdb_dbi_open(txn, nullptr, 0, &dbi);
	// Завершение транзакции
//	mdb_txn_commit(txn);

	// Закрытие LMDB базы данных
//	mdb_dbi_close(env, dbi);

}

BackIndex::~BackIndex() {
	cout << "dest: " << env << endl;
	if (env != NULL) {
//		cout << "txn" << txn << endl;
		mdb_txn_commit(txn);
		mdb_dbi_close(env, dbi);
		mdb_env_close(env);
	}
}

void updateOrInsertValue(MDB_env* env, MDB_txn* txn, MDB_dbi& dbi, const std::string& key, const std::string& aPath) {
    MDB_val mdbKey, mdbValue;

    // Открытие базы данных (DBI)
//    mdb_dbi_open(txn, nullptr, 0, &dbi);
    // Создание ключа и значения
    mdbKey.mv_size = key.size();
    mdbKey.mv_data = (void*)key.c_str();
    // Попытка найти значение по ключу
    int result = mdb_get(txn, dbi, &mdbKey, &mdbValue);
    if (result == MDB_NOTFOUND) {
        // Значение не найдено, создаем новую запись
        mdbValue.mv_size = aPath.size(); // Длина начального значения
        mdbValue.mv_data = (void*)aPath.c_str();
        mdb_put(txn, dbi, &mdbKey, &mdbValue, 0);
    } else if (result == 0) {
        std::string existingValue(static_cast<char*>(mdbValue.mv_data), mdbValue.mv_size);
        existingValue += "," + aPath;
        mdbValue.mv_size = existingValue.size();
        mdbValue.mv_data = (void*)existingValue.c_str();
        mdb_put(txn, dbi, &mdbKey, &mdbValue, 0);
    } else {
        // Обработка ошибки
        std::cerr << "Error: " << mdb_strerror(result) << std::endl;
    }

    // Закрытие DBI
//    mdb_dbi_close(env, dbi);
}



bool BackIndex::Add(const string& word, const string& actionPath, bool com) {
//	mdb_txn_begin(env, nullptr, 0, &txn);
	// Вызываем функцию для обновления или добавления значения
	updateOrInsertValue(env, txn, dbi, word, actionPath);
	// Завершение транзакции и закрытие окружения
//	mdb_txn_commit(txn);

	if (com) {
		mdb_txn_commit(txn);
		mdb_txn_begin(env, nullptr, 0, &txn);
	}
}

bool BackIndex::AddVec(vector<pair<string, string>>& vec) {
        MDB_txn* txn;
        mdb_txn_begin(env, nullptr, 0, &txn);
	for (auto el : vec) {
		// Вызываем функцию для обновления или добавления значения
		updateOrInsertValue(env, txn, dbi, el.first, el.second);
	}
        // Завершение транзакции и закрытие окружения
        mdb_txn_commit(txn);

}

