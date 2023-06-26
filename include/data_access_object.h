#include <sqlite3.h>
#include <helper.h>


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

    std::vector<std::string> execute_query(const std::string& query) {
        char* zErrMsg = nullptr;
        std::vector<std::string> results;
        int rc = sqlite3_exec(db, query.c_str(), callback, &results, &zErrMsg);
        if(rc != SQLITE_OK) {
            std::cout << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
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
