#include "BackupFileParser.h"

#include "functions/Utils.h"
#include "ios/IOSBackupParser.h"
#include "win/WINBackupParser.h"

using namespace wechat;

void parser::BackupFileParser::updateLoginUserRecord(model::WeChatLoginUser& user)
{
    int totalCount = 0, firstTime = INT_MAX, lastTime = INT_MIN;
    for (auto& f : user.getFriends())
    {
        totalCount += f.RecordCount();
        if (firstTime > f.BeginTime())
        {
            firstTime = f.BeginTime();
        }
        if (lastTime < f.LastTime())
        {
            lastTime = f.LastTime();
        }
    }
    user.setRecordCount(totalCount);
    user.setBeginTime(firstTime);
    user.setLastTime(lastTime);
}

static model::BackupType detectBackupType(const std::string& path)
{
    if (Utils::isFileExist(path + "/Manifest.db")) {
        return model::BackupType::BackupType_IOS;
    }
    return model::BackupType::BackupType_WIN;
}

parser::BackupFileParser* parser::createParser(const std::string& path)
{
    model::BackupType type = detectBackupType(path);
    if (type == model::BackupType::BackupType_IOS)
    {
        return new parser::ios::IOSBackupParser(path);
    }
    else if (type == model::BackupType::BackupType_WIN) {
        return new parser::win::WINBackupParser(path);
    }
    return nullptr;
}