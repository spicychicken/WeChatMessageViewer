#include "Sqlite3DBReader.h"

sqlitedb::StmtReader::StmtReader(sqlite3_stmt* stmt) : statement(stmt)
{

}

sqlitedb::StmtReader::~StmtReader()
{
    if (statement)
    {
        sqlite3_finalize(statement);
    }
}

void sqlitedb::StmtReader::setStmt(sqlite3_stmt* stmt)
{
    statement   = stmt;
}

bool sqlitedb::StmtReader::next() {
    return sqlite3_step(statement) == SQLITE_ROW;
}

int sqlitedb::StmtReader::readInt(int index)
{
    return sqlite3_column_int(statement, index);
}

std::string sqlitedb::StmtReader::readString(int index)
{
    const char* text = reinterpret_cast<const char*>(sqlite3_column_text(statement, index));
    if (text != nullptr) {
        return std::string(text);
    }
    return "";
}

std::string sqlitedb::StmtReader::readBlob(int index)
{
    const void* data = sqlite3_column_blob(statement, index);
    int length = sqlite3_column_bytes(statement, index);
    return std::string((const char*)data, length);
}

static bool connectionPoolEnabled = true;
static std::unordered_map<std::string, sqlite3*> connectionPool;

sqlitedb::DBPool::DBPool()
{
    connectionPoolEnabled = true;
}

sqlitedb::DBPool::~DBPool()
{
    for (auto& c : connectionPool)
    {
        sqlite3_close_v2(c.second);
    }
    connectionPool.erase(connectionPool.begin(), connectionPool.end());

    connectionPoolEnabled = false;
}

static std::shared_ptr<sqlite3> getConnectionOrCreate(const std::string& path)
{
    if ((connectionPoolEnabled && connectionPool.count(path) == 0) || !connectionPoolEnabled)
    {
        sqlite3* tmpDB = nullptr;
        if (sqlite3_open_v2(path.c_str(), &tmpDB, SQLITE_OPEN_READONLY, nullptr) == SQLITE_OK)
        {
            if (connectionPoolEnabled)
            {
                connectionPool.insert(std::make_pair(path, tmpDB));
                return std::shared_ptr<sqlite3>(tmpDB, [=](sqlite3* ptr) {});
            }
            else
            {
                return std::shared_ptr<sqlite3>(tmpDB, [=](sqlite3* ptr) { sqlite3_close_v2(ptr); });
            }
        }
        return std::shared_ptr<sqlite3>(tmpDB, [=](sqlite3* ptr) {});
    }
    else
    {
        return std::shared_ptr<sqlite3>(connectionPool[path], [=](sqlite3* ptr) {});
    }
}

int sqlitedb::queryCount(const std::string& path, const std::string& sql)
{
    std::shared_ptr<sqlite3> database = getConnectionOrCreate(path);
    if (database.get() != nullptr)
    {
        sqlite3_stmt* tmpStmt = nullptr;
        if (sqlite3_prepare_v2(database.get(), sql.c_str(), (int)(sql.size()), &tmpStmt, nullptr) == SQLITE_OK)
        {
            StmtReader reader(tmpStmt);
            reader.next();
            return reader.readInt(0);
        }
    }
    return 0;
}

bool sqlitedb::queryData(const std::string& path, const std::string& sql, sqlitedb::StmtReader& reader)
{
    std::shared_ptr<sqlite3> database = getConnectionOrCreate(path);
    if (database.get() != nullptr)
    {
        sqlite3_stmt* tmpStmt = nullptr;
        if (sqlite3_prepare_v2(database.get(), sql.c_str(), (int)(sql.size()), &tmpStmt, nullptr) == SQLITE_OK)
        {
            reader.setStmt(tmpStmt);
            return true;
        }
    }
    return false;
}