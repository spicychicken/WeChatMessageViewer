#ifndef SQLITE3DBREADER_H
#define SQLITE3DBREADER_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <tuple>
#include <unordered_map>

// #include "sqlite3.h"
#include "sqlcipher/sqlite3.h"

namespace sqlitedb
{

// single thread safe
class DBPool
{
public:
    DBPool();
    ~DBPool();
};

class StmtReader
{
public:
    StmtReader() = default;
    StmtReader(sqlite3_stmt* stmt);
    void setStmt(sqlite3_stmt* stmt);
    ~StmtReader();
    bool next();
    int readInt(int index);
    std::string readString(int index);
    std::string readBlob(int index);

private:
    sqlite3_stmt*       statement = nullptr;
};

int queryCount(const std::string& path, const std::string& sql);
bool queryData(const std::string& path, const std::string& sql, StmtReader& reader);

int queryCount(const std::string& path, const std::string& password, const std::string& sql);
bool queryData(const std::string& path, const std::string& password, const std::string& sql, StmtReader& reader);

}
#endif