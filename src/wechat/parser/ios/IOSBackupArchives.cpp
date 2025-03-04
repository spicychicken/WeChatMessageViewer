#include "IOSBackupArchives.h"

#include <algorithm>
#include <chrono>

#include "sqlite3cpp/Sqlite3DBReader.h"
#include "Constants.h"
#include "functions/Utils.h"

using std::string;
using std::vector;
using std::function;

using wechat::parser::ios::ManifestRecord;
using wechat::parser::ios::IOSBackupArchives;

constexpr static char IOS_MANIFEST_FILE[] = "Manifest.db";

void IOSBackupArchives::setArchivesPath(const std::string& path)
{
    folderName      = path;
    manifestPath    = folderName + Constants::BACKSLASH + IOS_MANIFEST_FILE;
}

vector<ManifestRecord> IOSBackupArchives::getManifestRecordsFromDB(const string& countSql, const string& querySql) const
{
    int count = sqlitedb::queryCount(manifestPath, countSql);

    if (count != 0)
    {
        sqlitedb::StmtReader    reader;
        if (sqlitedb::queryData(manifestPath, querySql, reader))
        {
            vector<ManifestRecord>  records(count);
            for (int i = 0; i < count && reader.next(); ++i)
            {
                records[i].fileID       = reader.readString(0);
                records[i].relativePath = reader.readString(2);
                records[i].flags        = reader.readInt(3);
                records[i].file         = reader.readString(4);
            }
            return records;
        }
    }
    return {};
}

// manifest
vector<ManifestRecord> IOSBackupArchives::getManifestRecordsByDomainAndStartEndWith(const string& domain, 
                                    const string& starts, const string& ends)
{
    const std::string countSql = "select count(*) from Files where relativePath like '" + starts + "%" + ends + "'";
    const std::string querySql = "select * from Files where relativePath like '" + starts + "%" + ends + "'";

    return getManifestRecordsFromDB(countSql, querySql);
}

std::vector<ManifestRecord> IOSBackupArchives::getAllManifestRecordsByDomain(const string& domain) const
{
    const string countSql = "select count(*) from Files where domain = '" + domain + "'";
    const string querySql = "SELECT * FROM Files where domain='" + domain + "'";

    return getManifestRecordsFromDB(countSql, querySql);
}

std::vector<ManifestRecord> IOSBackupArchives::getAllManifestRecords() const {
    const string countSql = "select count(*) from Files";
    const string querySql = "SELECT * FROM Files";

    return getManifestRecordsFromDB(countSql, querySql);
}

bool IOSBackupArchives::getManifestRecordByRelativePath(const string& relativePath, ManifestRecord& record) const
{
    const string countSql = "select count(*) from Files where relativePath='" + relativePath + "'";
    const string querySql = "select * from Files where relativePath='" + relativePath + "'";

    vector<ManifestRecord> results = getManifestRecordsFromDB(countSql, querySql);
    if (results.size() == 1)
    {
        record = results[0];
        return true;
    }
    return false;
}

bool IOSBackupArchives::getAbsolutePathByRelativePath(const std::string& relativePath, std::string& absolute) const
{
    ManifestRecord  record;
    if (getManifestRecordByRelativePath(relativePath, record))
    {
        absolute = getAbsolutePathByManifestRecord(record);
        return true;
    }
    return false;
}

bool IOSBackupArchives::getFileContentByRelativePath(const std::string& relativePath, std::string& content) const
{
    ManifestRecord  record;
    if (getManifestRecordByRelativePath(relativePath, record))
    {
        return getFileContentByManifestRecord(record, content);
    }
    return false;
}

string IOSBackupArchives::getAbsolutePathByManifestRecord(const ManifestRecord& record) const
{
    return getAbsolutePathFromFileID(record.fileID);
}

bool IOSBackupArchives::getFileContentByManifestRecord(const ManifestRecord& record, std::string& content) const
{
    string absolutePath = getAbsolutePathByManifestRecord(record);
    if (absolutePath != "" && Utils::isFileExist(absolutePath))
    {
        content = Utils::readBinaryFile(absolutePath);
        return true;
    }
    return false;
}

bool IOSBackupArchives::getFileContentByPath(const std::string& path, std::string& content) const
{
    string filePath = folderName + Constants::BACKSLASH + path;
    if (Utils::isFileExist(filePath))
    {
        content = Utils::readBinaryFile(filePath);
        return true;
    }
    return false;
}

vector<ManifestRecord> filterBy(const vector<ManifestRecord>& records, std::function<bool (ManifestRecord)> func)
{
    vector<ManifestRecord> results;
    for (auto record : records)
    {
        if (func(record))
        {
            results.push_back(record);
        }
    }
    return results;
}

string IOSBackupArchives::getAbsolutePathFromFileID(const string& fileID) const
{
    return folderName + Constants::BACKSLASH + fileID.substr(0, 2) + Constants::BACKSLASH + fileID;
}

/*
string IOSBackupArchives::getAbsoluteByRelativePathOr(const string& relativePath, const string& defaultPath) const
{
    string absolutePath = getAbsoluteByRelativePath(relativePath);
    if (absolutePath != "" && Utils::isFileExist(absolutePath))
    {
        return absolutePath;
    }
    return defaultPath;
}
*/