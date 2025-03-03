#include "WeChatContext.h"

#include <iostream>
#include "parser/BackupFileParser.h"
#include "functions/Utils.h"

#include "audio/OpenAL.h"

using wechat::WeChatContext;
using std::string;
using std::vector;
using wechat::model::WeChatMessage;
using wechat::model::WeChatLoginUser;
using wechat::model::WeChatFriend;
using wechat::model::BackupType;

WeChatContext& WeChatContext::get()
{
    static WeChatContext context;
    return context;
}

BackupType WeChatContext::detectBackupType(const string& path)
{
    return parser::detectBackupType(path);
}

bool WeChatContext::initContextFromPath(const string& path)
{
    // do necessary cleanup
    if (backupParser)
    {
        delete backupParser;
    }

    backup.cleanup();

    backupParser = parser::createParser(path);
    if (backupParser) {
        return backupParser->loadBackup(backup);
    }
    return false;
}

vector<string> WeChatContext::listLoginUserNames()
{
    return backupParser->listLoginUserNames(backup);
}

WeChatLoginUser* WeChatContext::loadLoginUser(const string& loginUserName, const string& secretKey)
{
    WeChatLoginUser* currentLoginUser = &backupParser->loadLoginUser(backup, loginUserName, secretKey);
    switchCurrentLoginUser(currentLoginUser);
    return currentLoginUser;
}

const WeChatFriend& WeChatContext::getFriendByID(const string& friendID)
{
    // after loadLoginUser and listFriends
    return currentUser->getFriend(friendID);
}

const vector<WeChatFriend>& WeChatContext::listFriends()
{
    if (currentUser && !currentUser->hasFriendData())
    {
        backupParser->loadUserFriends(*currentUser);
    }

    return currentUser->getFriends();
}

vector<WeChatMessage> WeChatContext::listMessages(const string& friendID, int page, int count)
{
    auto& aFriend = currentUser->getFriend(friendID);
    return sParser->loadFriendMessages(*currentUser, aFriend, page, count);
}

void WeChatContext::playAudio(const std::string& friendID, const WeChatMessage& message)
{
    if (backup.getBackupType() == BackupType::BackupType_WIN)
    {
        auto& aFriend = currentUser->getFriend(friendID);
        auto data = backupParser->loadUserAudioData(*currentUser, aFriend, message);
        audio::OpenAL::singlePlaySilkFromData(data);
    }
    else {
        if (!message.getMetadata("src").empty())
        {
            audio::OpenAL::singlePlaySilkFromPath(message.getMetadata("src"));
        }
    }
}

std::string WeChatContext::loadMsgImgData(const std::string& fileName)
{
    return backupParser->loadMsgImgData(fileName);
}

// --------------------------------------------------------------------------------
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