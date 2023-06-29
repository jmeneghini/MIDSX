#ifndef DATAACCESSOBJECT_H
#define DATAACCESSOBJECT_H

#include <sqlite3.h>
#include <helper.h>

// Exception class for handling database errors
class DatabaseException : public std::exception {
public:
    explicit DatabaseException(const std::string& message): msg_(message) {}
    const char* what() const noexcept override { return msg_.c_str(); }
private:
    std::string msg_;
};


class DataAccessObject {
public:
    explicit DataAccessObject(const std::string& db_name) {
        int rc = sqlite3_open(db_name.c_str(), &db);
        if(rc) {
            std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    ~DataAccessObject() {
        sqlite3_close(db);
    }

    std::vector<std::string> executeQuery(const std::string& query) {
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


private:
    sqlite3* db{};

    static int callback(void* data, int argc, char** argv, char** azColName) {
        auto* results = static_cast<std::vector<std::string>*>(data);
        for(int i = 0; i < argc; i++){
            results->emplace_back(argv[i]);
        }
        return 0;
    }
};

#endif //DATAACCESSOBJECT_H