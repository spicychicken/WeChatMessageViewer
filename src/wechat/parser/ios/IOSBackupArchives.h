#ifndef IOSBACKUPARCHIVES_H
#define IOSBACKUPARCHIVES_H

#include <string>
#include <vector>
#include <functional>

namespace wechat::parser::ios
{

struct ManifestRecord
{
    std::string fileID;
    std::string relativePath;
    int         flags;
    std::string file;
};

class IOSBackupArchives
{
public:
    void setArchivesPath(const std::string& path);

public:
    std::vector<ManifestRecord> getManifestRecordsByDomainAndStartEndWith(const std::string& domain, 
                                        const std::string& starts, const std::string& ends);
    std::vector<ManifestRecord> getAllManifestRecordsByDomain(const std::string& domain) const;
    std::vector<ManifestRecord> getAllManifestRecords() const;

    bool getManifestRecordByRelativePath(const std::string& relativePath, ManifestRecord& record) const;
    bool getAbsolutePathByRelativePath(const std::string& relativePath, std::string& absolute) const;
    bool getFileContentByRelativePath(const std::string& relativePath, std::string& content) const;

    std::string getAbsolutePathByManifestRecord(const ManifestRecord& record) const;
    bool getFileContentByManifestRecord(const ManifestRecord& record, std::string& content) const;
    bool getFileContentByPath(const std::string& path, std::string& content) const;

private:
    std::string getAbsolutePathFromFileID(const std::string& fileID) const;
    std::vector<ManifestRecord> getManifestRecordsFromDB(const std::string& countSql, const std::string& querySql) const;

private:
    std::string                     folderName;
    std::string                     manifestPath;
};

}

#endif // IOSBACKUPARCHIVES_H
