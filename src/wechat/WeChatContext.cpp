#include "WeChatContext.h"

#include <iostream>
#include "parser/BackupFileParser.h"
#include "functions/Utils.h"

using wechat::WeChatContext;
using std::string;
using std::vector;
using wechat::model::WeChatLoginUser;
using wechat::model::BackupType;

WeChatContext& WeChatContext::get()
{
    static WeChatContext context;
    return context;
}

BackupType WeChatContext::detectBackupType(const std::string& path)
{
    return parser::detectBackupType(path);
}

bool WeChatContext::initContextFromPath(const std::string& path)
{
    backupParser = parser::createParser(path);
    if (backupParser) {
        return backupParser->loadBackup(backup);
    }
    return false;
}

vector<string> WeChatContext::listLoginUsers()
{
    return backupParser->listLoginUsers(backup);
}

WeChatLoginUser* WeChatContext::loadLoginUser(const std::string& loginUserName, const std::string& secretKey)
{
    WeChatLoginUser* currentLoginUser = &backupParser->loadLoginUser(backup, loginUserName, secretKey);
    switchCurrentLoginUser(currentLoginUser);
    return currentLoginUser;
}

vector<string> WeChatContext::listFriends()
{
    if (currentUser && !currentUser->hasFriendData())
    {
        backupParser->loadUserFriends(*currentUser);
    }

    vector<string>    friends;
    for (const auto& iter : currentUser->getFriends())
    {
        friends.push_back(iter.UserName());
    }
    return friends;
}

void WeChatContext::createParserFromPath(const string& path)
{
    backupParser = parser::createParser(path);
}

void WeChatContext::loadBackup()
{
    backupParser->loadBackup(backup);
}

void WeChatContext::loadLoginUsers()
{
    backupParser->loadLoginUsers(backup);
}

void WeChatContext::loadCurrentLoginUserFriends()
{
    backupParser->loadUserFriends(*currentUser);
}

void WeChatContext::switchCurrentLoginUser(model::WeChatLoginUser* newLoginUser)
{
    if ((currentUser != nullptr && currentUser != newLoginUser) || (currentUser == nullptr))
    {
        currentUser = newLoginUser;
    }
}

const std::unordered_map<string, WeChatLoginUser> WeChatContext::getLoginUsers() const
{
    return backup.getLoginUsers();
}