#ifndef WINBACKUPARCHIVES_H
#define WINBACKUPARCHIVES_H

#include <string>
#include <vector>
#include <functional>

namespace wechat::parser::win
{

class WINBackupArchives
{
public:
    void setArchivesPath(const std::string& path);

public:
    std::vector<std::string> listLoginUserNames();
    std::vector<std::string> listMsgDBNames(const std::string& userName);
    std::vector<std::string> listAudioDBNames(const std::string& userName);

    std::vector<std::string> listUsedUserNameFor(const std::string& newUserName);

    std::string correctPath(const std::string& userName, const std::string& path);

private:
    std::string                     folderName;
};

}

#endif // WINBACKUPARCHIVES_H
