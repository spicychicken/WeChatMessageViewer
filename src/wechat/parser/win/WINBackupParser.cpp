#include "WINBackupParser.h"

#include <iostream>
#include <chrono>

#include "json/json.h"

#include "md5/md5.h"
#include "sqlite3cpp/Sqlite3DBReader.h"
#include "functions/Protobuf.h"
#include "functions/Utils.h"
#include "functions/Plist.h"
#include "functions/Mmkv.h"

#include "WINMessageParser.h"

#include "internal/details.h"

using std::string;
using std::unordered_map;
using std::to_string;
using std::vector;

using wechat::model::WeChatBackup;
using wechat::model::WeChatLoginUser;
using wechat::model::WeChatFriend;
using wechat::model::WeChatMessage;
using wechat::parser::win::WINMessageParser;

using namespace wechat::parser::win;

WINBackupParser::WINBackupParser(const string& path) : backupPath(path)
{
}

string WINBackupParser::getDBPassword(const string& userName)
{
    return rawKeys.count(userName) == 0 ? defaultPass : rawKeys[userName];
}

static void loadDBPassFromLocalToMap(unordered_map<string, string>& rawKeys)
{
    string jsonContent = Utils::readFile(Utils::getCurrentPath() + "/dbpass.json");

    Json::Value root;
    Json::CharReaderBuilder crb;
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
    string err;
    if (cr->parse(jsonContent.c_str(), jsonContent.c_str() + jsonContent.length(), &root, &err))
    {
        Json::Value::Members member = root.getMemberNames(); 
        for (auto m : member)
        {
            rawKeys.insert(std::make_pair(m, root[m].asString()));
        }
    }
}

void WINBackupParser::loadBackup(model::WeChatBackup& backup)
{
    backup.setBackupType(wechat::model::BackupType::BackupType_WIN);

    // load dbpass from local json
    loadDBPassFromLocalToMap(rawKeys);

    // detected runtime wechat pass
    defaultPass = details::detectSqliteRawKey();
}

static vector<string> listLoginUserFolder(const string& backupPath)
{
    vector<string> folders;
    for (auto& folder : Utils::listSubFolder(backupPath))
    {
        if (Utils::isFolderExist(backupPath + "/" + folder + "/Msg"))
        {
            folders.push_back(folder);
        }
    }
    return folders;
}

void WINBackupParser::loadLoginUsers(WeChatBackup& backup)
{
    unordered_map<string, WeChatLoginUser> users;

    vector<string> userFolders = listLoginUserFolder(backupPath);
    for (auto& userName : userFolders)
    {
        string userID = md5(userName);
        users[userID] = WeChatLoginUser();
        users[userID].setUserName(userName);

        loadUserFriendFromContact(userName, users[userID]);
    }
    backup.setLoginUsers(std::move(users));
}

void WINBackupParser::loadUserFriends(WeChatLoginUser& user)
{
    unordered_map<string, WeChatFriend> friends;
    loadUserFriendsFromSession(user, friends);
    loadUserFriendsDetailsFromContact(user, friends);
    loadUserFriendsDetailsFromMsg(user, friends);

    user.setFriends(std::move(friends));
    updateLoginUserRecord(user);
}

void WINBackupParser::loadUserFriendsFromSession(const WeChatLoginUser& user, std::unordered_map<string, WeChatFriend>& friends)
{
    string dbPath = backupPath + "/" + user.UserName() + "/Msg/MicroMsg.db";
    string sessionSql = "select strUsrName,strNickName,nTime from Session";

    sqlitedb::StmtReader reader;
    if (sqlitedb::queryData(dbPath, getDBPassword(user.UserName()), sessionSql, reader))
    {
        while (reader.next())
        {
            string friendName = reader.readString(0);
            string friendID = md5(friendName);

            WeChatFriend& afriend = friends[friendID];
            afriend.setUserID(friendID);
            afriend.setUserName(friendName);
            afriend.setNickName(reader.readString(1));
            afriend.setBeginTime(reader.readInt(2));
            afriend.setLastTime(afriend.BeginTime());

            afriend.setLocalHeadImg(LOCAL_DEFAULT_HEAD_IMAGE);
        }
    }
}

void WINBackupParser::loadUserFriendsDetailsFromContact(const WeChatLoginUser& user, std::unordered_map<string, WeChatFriend>& friends)
{
    string dbPath = backupPath + "/" + user.UserName() + "/Msg/MicroMsg.db";
    string contactSql = "select c.UserName,c.Alias,c.Remark,c.NickName,img.smallHeadImgUrl,img.bigHeadImgUrl from Contact as c, ContactHeadImgUrl as img where c.UserName == img.usrName";

    sqlitedb::StmtReader reader;
    if (sqlitedb::queryData(dbPath, getDBPassword(user.UserName()), contactSql, reader))
    {
        while (reader.next())
        {
            string friendName = reader.readString(0);
            string friendID = md5(friendName);

            if (friends.count(friendID))
            {
                WeChatFriend& afriend = friends[friendID];
                afriend.setUserID(friendID);
                afriend.setUserName(friendName);
                afriend.setAliasName(reader.readString(1));
                afriend.setRemark(reader.readString(2));
                afriend.setNickName(reader.readString(3));
                afriend.setHeadImgUrl(reader.readString(4));
                afriend.setHeadImgUrlHD(reader.readString(5));
            }
        }
    }
}

static vector<string> getMsgDBNames(const string& rootPath)
{
    vector<string> dbNames;
    for (auto& folder : Utils::listSubFiles(rootPath))
    {
        if (Utils::startsWith(folder, "MSG") && Utils::endsWith(folder, ".db"))
        {
            dbNames.push_back(folder);
        }
    }
    return dbNames;
}

void WINBackupParser::loadUserFriendsDetailsFromMsg(const WeChatLoginUser& user, unordered_map<string, WeChatFriend>& friends)
{
    vector<string> dbNames = getMsgDBNames(backupPath + "/" + user.UserName() + "/Msg/Multi");
    for (auto& dbName : dbNames)
    {
        string countSql = "select StrTalker, count(*), MIN(CreateTime), MAX(CreateTime) from MSG group by talkerId";
        string msgDBPath = backupPath + "/" + user.UserName() + "/Msg/Multi/" + dbName;

        sqlitedb::StmtReader reader;
        if (sqlitedb::queryData(msgDBPath, getDBPassword(user.UserName()), countSql, reader))
        {
            while (reader.next())
            {
                string friendName = reader.readString(0);
                string friendID = md5(friendName);

                if (friends.count(friendID))
                {
                    WeChatFriend& afriend = friends[friendID];

                    int count = reader.readInt(1);
                    afriend.setRecordCount(afriend.RecordCount() + count);

                    int beginTime = reader.readInt(2), endTime = reader.readInt(3);
                    if (afriend.BeginTime() == 0 || afriend.BeginTime() > beginTime) {
                        afriend.setBeginTime(beginTime);
                    }
                    if (afriend.LastTime() == 0 || afriend.LastTime() < endTime) {
                        afriend.setLastTime(endTime);
                    }

                    afriend.appendDBPathAndCount(dbName, count);
                }
            }
        }
    }
}

void WINBackupParser::loadGroupMembers(const WeChatLoginUser& user, WeChatFriend& afriend)
{
    string dbPath = backupPath + "/" + user.UserName() + "/Msg/MicroMsg.db";
    string contactSql = "select UserNameList from ChatRoom where ChatRoomName='" + afriend.UserName() + "'";

    sqlitedb::StmtReader reader;
    if (sqlitedb::queryData(dbPath, getDBPassword(user.UserName()), contactSql, reader))
    {
        if (reader.next())
        {
            for (auto& memberName : Utils::split(reader.readString(0), "^G")) {
                string memberID = md5(memberName);
                WeChatFriend& member = afriend.getMember(memberID);
                member.setUserName(memberName);
                loadUserFriendFromContact(user.UserName(), member);
            }
        }
    }
}

void WINBackupParser::loadGroupMember(const WeChatLoginUser& user, WeChatFriend& afriend, const string& memberName)
{
    string memberID = md5(memberName);
    WeChatFriend& member = afriend.getMember(memberID);
    member.setUserName(memberName);

    loadUserFriendFromContact(user.UserName(), member);
}

static std::pair<int, int> getDBStartIndexByPageAndCount(const vector<int>& dbCounts, int page, int countPerPage)
{
    int sum = 0, offset = (page - 1) * countPerPage;
    for (int i = 0; i < dbCounts.size(); ++i)
    {
        if (offset >= sum && offset < (dbCounts[i] + sum))
        {
            return std::make_pair(i, offset - sum);
        }
        sum += dbCounts[i];
    }
    return std::make_pair(-1, -1);
}

vector<WeChatMessage> WINBackupParser::loadFriendMessages(const WeChatLoginUser& user, WeChatFriend& afriend, int page, int countPerPage)
{
    vector<string> dbPaths = afriend.DbPaths();
    if (!dbPaths.empty())
    {
        auto [p, offset] = getDBStartIndexByPageAndCount(afriend.DbCounts(), page, countPerPage);
        if (p != -1)
        {
            WINMessageParser        messageParser(user, afriend, this, backupPath);
            vector<WeChatMessage>   messages;

            string dbPath = backupPath + "/" + user.UserName() + "/Msg/Multi/" + dbPaths[p];
            string contactSql = "select Type,IsSender,StrContent,CreateTime,BytesExtra from MSG where StrTalker='" + afriend.UserName() + "'";
            contactSql = contactSql + " order by CreateTime limit " + std::to_string(countPerPage) + " offset " + std::to_string(offset);

            sqlitedb::StmtReader reader;
            if (sqlitedb::queryData(dbPath, getDBPassword(user.UserName()), contactSql, reader))
            {
                while (reader.next())
                {
                    messages.push_back(messageParser.parse(
                        reader.readInt(3), reader.readString(2), reader.readInt(1) == 1, reader.readInt(0), reader.readBlob(4)));
                }
            }

            return messages;
        }
    }
    return {};
}

bool WINBackupParser::loadUserFriendFromContact(const string& userName, model::WeChatUser& afriend)
{
    string dbPath = backupPath + "/" + userName + "/Msg/MicroMsg.db";
    string contactSql = "select c.UserName,c.Alias,c.Remark,c.NickName,img.smallHeadImgUrl,img.bigHeadImgUrl ";
    contactSql += "from Contact as c, ContactHeadImgUrl as img where c.UserName == img.usrName and c.UserName = '";
    contactSql += (afriend.UserName() + "'");

    sqlitedb::StmtReader reader;
    if (sqlitedb::queryData(dbPath, getDBPassword(userName), contactSql, reader))
    {
        if (reader.next())
        {
            afriend.setAliasName(reader.readString(1));
            afriend.setSignature(reader.readString(2));
            afriend.setNickName(reader.readString(3));
            afriend.setHeadImgUrl(reader.readString(4));
            afriend.setHeadImgUrl(reader.readString(5));
            afriend.setLocalHeadImg(LOCAL_DEFAULT_HEAD_IMAGE);

            return true;
        }
    }
    return false;
}