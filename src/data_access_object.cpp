#include "data_access_object.h"
#include <iostream>
#include <sqlite3.h>

DatabaseException::DatabaseException(const std::string& message) : msg_(message) {}

const char* DatabaseException::what() const noexcept { return msg_.c_str(); }

DataAccessObject::DataAccessObject(const std::string& db_name) {
    int rc = sqlite3_open(db_name.c_str(), &db);
    if(rc) {
        std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    }
}

DataAccessObject::~DataAccessObject() {
    sqlite3_close(db);
}

std::vector<std::string> DataAccessObject::executeQuery(const std::string& query) {
    char* zErrMsg = nullptr;
    std::vector<std::string> results;
    int rc = sqlite3_exec(db, query.c_str(), callback, &results, &zErrMsg);

    if(rc != SQLITE_OK) {
        std::string errMsg;
        if (zErrMsg) {
            errMsg = zErrMsg;
            sqlite3_free(zErrMsg);
        } else {
            errMsg = "Unknown SQL error";
        }
        throw DatabaseException(errMsg);
    }

    return results;
}

int DataAccessObject::callback(void* data, int argc, char** argv, char** azColName) {
    auto* results = static_cast<std::vector<std::string>*>(data);
    for(int i = 0; i < argc; i++){
        results->emplace_back(argv[i]);
    }
    return 0;
}
