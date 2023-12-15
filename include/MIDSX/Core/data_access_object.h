#ifndef DATAACCESSOBJECT_H
#define DATAACCESSOBJECT_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <exception>

/**
 * @brief Exception class for database errors.
 */
class DatabaseException : public std::exception {
public:
    explicit DatabaseException(std::string  message);
    const char* what() const noexcept override;
private:
    std::string msg_;
};

/**
 * @brief Class which provides an interface to a SQLite database.
 */
class DataAccessObject {
public:
    /**
     * @brief Constructor for the DataAccessObject class.
     *
     * @param db_name The name of the database.
     */
    explicit DataAccessObject(const std::string& db_name);
    ~DataAccessObject();

    /**
     * @brief Executes a query and returns the results as a vector of strings
     *
     * @param query The query to execute.
     * @return A vector of strings containing the results of the query.
     */
    std::vector<std::string> executeQuery(const std::string& query);

private:
    sqlite3* db{};
    static int callback(void* data, int argc, char** argv, char** azColName);
};

#endif //DATAACCESSOBJECT_H
