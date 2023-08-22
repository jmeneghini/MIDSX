#ifndef DATAACCESSOBJECT_H
#define DATAACCESSOBJECT_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <exception>

// Exception class for handling database errors
class DatabaseException : public std::exception {
public:
    explicit DatabaseException(std::string  message);
    const char* what() const noexcept override;
private:
    std::string msg_;
};

class DataAccessObject {
public:
    explicit DataAccessObject(const std::string& db_name);
    ~DataAccessObject();

    // Executes a query and returns the results as a vector of strings
    std::vector<std::string> executeQuery(const std::string& query);

private:
    sqlite3* db{};
    static int callback(void* data, int argc, char** argv, char** azColName);
};

#endif //DATAACCESSOBJECT_H
