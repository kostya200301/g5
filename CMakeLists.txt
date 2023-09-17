cmake_minimum_required(VERSION 3.10)
project(YourProjectName)


set(CMAKE_CXX_STANDARD 17)



# Укажите исходные файлы вашего проекта
set(SOURCES
    src/BackIndexRock.cpp
    src/main.cpp
    src/Baket.cpp
    src/BTree.cpp
    src/NER.cpp
    src/Searcher.cpp
)

# find_package(gflags CONFIG REQUIRED)



# Включите модуль Fetch-content
include(FetchContent)
# Задайте версию RocksDB
set(ROCKSDB_VERSION "8.5.3")
# Загрузите и соберите RocksDB
FetchContent_Declare(
    rocksdb
    GIT_REPOSITORY https://github.com/facebook/rocksdb.git
    GIT_TAG v${ROCKSDB_VERSION}
)
FetchContent_GetProperties(rocksdb)
if(NOT rocksdb_POPULATED)
    FetchContent_Populate(rocksdb)
    # Создайте подкаталог для сборки RocksDB
    set(rocksdb_build_dir "${CMAKE_BINARY_DIR}/external/rocksdb-build")
    file(MAKE_DIRECTORY ${rocksdb_build_dir})
    # Задайте опции сборки RocksDB
    set(rocksdb_cmake_args
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external/rocksdb-install
    )
    # Соберите RocksDB
    execute_process(
        COMMAND ${CMAKE_COMMAND} ${rocksdb_SOURCE_DIR} ${rocksdb_cmake_args}
        WORKING_DIRECTORY ${rocksdb_build_dir}
    )
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${rocksdb_build_dir}
    )
endif()
# Определите пути к собранной версии RocksDB
set(ROCKSDB_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/_deps/rocksdb-src/include")
set(ROCKSDB_LIBRARIES "${CMAKE_BINARY_DIR}/external/rocksdb-build")



include_directories(./vcpkg/installed/x64-linux/include)
link_directories(./vcpkg/installed/x64-linux/lib)

find_package(Threads REQUIRED)




# Создайте исполняемый файл
add_executable(YourExecutableName ${SOURCES})
# Линковка с библиотекой RocksDB
target_include_directories(YourExecutableName PRIVATE ${ROCKSDB_INCLUDE_DIRS})
target_link_libraries(YourExecutableName
    ${ROCKSDB_LIBRARIES}/librocksdb.a  # Замените на фактический путь и имя библиотеки
    Threads::Threads
    sodium
#    nlohmann_json
#    gflags
)

