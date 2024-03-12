#include "WeChatContext.h"

#include <iostream>
#include "parser/BackupFileParser.h"
#include "functions/Utils.h"

using wechat::WeChatContext;
using std::string;
using wechat::model::WeChatLoginUser;

WeChatContext& WeChatContext::get()
{
    static WeChatContext context;
    return context;
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

bool WeChatContext::switchCurrentLoginUser(const string& newUserID)
{
    if ((currentUser != nullptr && currentUser->UserID() != newUserID) || (currentUser == nullptr))
    {
        currentUser = &backup.getLoginUserByID(newUserID);
        return true;
    }
    return false;
}

const std::unordered_map<string, WeChatLoginUser> WeChatContext::getLoginUsers() const
{
    return backup.getLoginUsers();
}