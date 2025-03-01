#include "IOSBackupParser.h"

#include <iostream>
#include <chrono>

#include "md5/md5.h"
#include "sqlite3cpp/Sqlite3DBReader.h"
#include "functions/Protobuf.h"
#include "functions/Utils.h"
#include "functions/Plist.h"
#include "functions/Mmkv.h"

#include "IOSBackupArchives.h"
#include "IOSMessageParser.h"

#include "internal/details.h"

using std::string;
using std::vector;
using std::unordered_map;
using std::to_string;
using std::vector;

using wechat::model::WeChatBackup;
using wechat::model::WeChatLoginUser;
using wechat::model::WeChatFriend;
using wechat::model::WeChatMessage;
using wechat::parser::ios::IOSBackupParser;
using wechat::parser::ios::IOSBackupArchives;
using wechat::parser::ios::IOSMessageParser;

using namespace wechat::parser::ios;

constexpr static char TENCENT_MANIFEST_DOMAIN[] = "AppDomain-com.tencent.xin";
constexpr static char TENCENT_SHARED_MANIFEST_DOMAIN[] = "AppDomainGroup-group.com.tencent.xin";

IOSBackupArchives   iosArchives;

IOSBackupParser::IOSBackupParser(const string& path) : backupPath(path)
{
    iosArchives.setArchivesPath(backupPath);
}

bool IOSBackupParser::loadBackup(model::WeChatBackup& backup)
{
    backup.setBackupType(wechat::model::BackupType::BackupType_IOS);

    string infoPlistContent;
    if (iosArchives.getFileContentByPath("Info.plist", infoPlistContent))
    {
        auto infoPlist = Plist::toMap(infoPlistContent);

        // [To-Do] change to metadatas
        // backup.setITuneVersion(infoPlist["iTunes Version"]);
        // backup.setProductVersion(infoPlist["Product Version"]);
        // backup.setLastBackupDate(infoPlist["Last Backup Date"]);
        return true;
    }
    return false;
}

vector<string> IOSBackupParser::listLoginUserNames(WeChatBackup& backup)
{
    unordered_map<string, WeChatLoginUser> users;
    loadLoginUsersFromMMDB(users);
    loadLoginUsersFromLoginInfo2(users);

    vector<string>    loginUserNames;
    for (const auto& pair : users)
    {
        loginUserNames.push_back(pair.second.UserName());
    }
    return loginUserNames;
}

WeChatLoginUser& IOSBackupParser::loadLoginUser(model::WeChatBackup& backup, const std::string& loginUserName, const std::string& secretKey)
{
    // [To-Do] refine later
    loadLoginUsers(backup);
    return backup.getLoginUserByID(md5(loginUserName));
}

void IOSBackupParser::loadLoginUsers(WeChatBackup& backup)
{
    unordered_map<string, WeChatLoginUser> users;
    loadLoginUsersFromMMDB(users);
    loadLoginUsersFromLoginInfo2(users);
    loadLoginUserDetailsFromMMsetting(users);
    backup.setLoginUsers(std::move(users));
}

void IOSBackupParser::loadLoginUsersFromMMDB(unordered_map<string, WeChatLoginUser>& users)
{
    // Documents/...../DB/MM.sqlite
    vector<ManifestRecord> records = iosArchives.getManifestRecordsByDomainAndStartEndWith(TENCENT_MANIFEST_DOMAIN, "Documents/", "/DB/MM.sqlite");

    for (ManifestRecord& record : records)
    {
        string userID = Utils::split(record.relativePath, "/")[1];
        if (users.count(userID) == 0)
        {
            users[userID] = WeChatLoginUser();
            users[userID].setUserID(userID);
        }
    }
}

void IOSBackupParser::loadLoginUsersFromLoginInfo2(unordered_map<string, model::WeChatLoginUser>& users)
{
    string loginInfoContent;
    if (iosArchives.getFileContentByRelativePath("Documents/LoginInfo2.dat", loginInfoContent))
    {
        unordered_map<string, string> usersFromLoginInfo2 = Protobuf::toMap(loginInfoContent);
        for (int i = 0; i < 10; ++i)
        {
            string userNameKey = "1." + to_string(i) + ".1.0";
            if (usersFromLoginInfo2.count(userNameKey) != 0)
            {
                string userName = usersFromLoginInfo2[userNameKey];
                string userID = md5(userName);

                if (users.count(userID) == 0)
                {
                    users[userID] = WeChatLoginUser();
                    users[userID].setUserID(userID);
                }
                users[userID].setUserName(userName);
                users[userID].setNickName(usersFromLoginInfo2["1." + to_string(i) + ".3.0"]);
                users[userID].setPhone(usersFromLoginInfo2["1." + to_string(i) + ".2.0"]);
            }
        }
    }
}

void IOSBackupParser::loadLoginUserDetailsFromMMsetting(std::unordered_map<string, WeChatLoginUser>& users)
{
    for (auto& user : users)
    {
        // Documents/{uid}/mmsetting.archive
        string mmSettingContent;
        if (iosArchives.getFileContentByRelativePath("Documents/" + user.second.UserID() + "/mmsetting.archive", mmSettingContent))
        {
            auto userSettingWithValue = Plist::arrayToMap(mmSettingContent);

            /* user.second.setUserName(userSettingWithValue["$objects.1.UsrName"]);
            user.second.setNickName(userSettingWithValue["$objects.1.NickName"]);
            user.second.setHeadImgUrl(getNSObjectsByKey(userSettingWithValue, "headimgurl"));
            user.second.setHeadImgUrlHD(getNSObjectsByKey(userSettingWithValue, "headhdimgurl"));   */

            user.second.setUserName(userSettingWithValue["$top.root.UsrName"]);
            user.second.setNickName(userSettingWithValue["$top.root.NickName"]);
            // user.second.setAliasName(userSettingWithValue["$top.root.AliasName"]);
            user.second.setCountry(userSettingWithValue["$top.root.Country"]);
            user.second.setProvince(userSettingWithValue["$top.root.Province"]);
            user.second.setCity(userSettingWithValue["$top.root.City"]);
            user.second.setPhone(userSettingWithValue["$top.root.Mobile"]);
            user.second.setEmail(userSettingWithValue["$top.root.Email"]);

            user.second.setHeadImgUrl(details::getNSObjectsByKey(userSettingWithValue, "headimgurl"));
            user.second.setHeadImgUrlHD(details::getNSObjectsByKey(userSettingWithValue, "headhdimgurl"));
        }
        else {
            string filePath;
            bool findFilePath = iosArchives.getAbsolutePathByRelativePath("Documents/MMappedKV/mmsetting.archive." + user.second.UserName(), filePath);
            string fileCrcPath;
            bool findFileCrcPath = iosArchives.getAbsolutePathByRelativePath("Documents/MMappedKV/mmsetting.archive." + user.second.UserName() + ".crc", fileCrcPath);

            if (findFilePath && findFileCrcPath)
            {
                // auto userMMKVSetting = Mmkv::toMap(filePath, fileCrcPath);

                /*
                    user.UserName = userMMKVSetting["86"]
                    user.NickName = userMMKVSetting["88"]
                    # 本机user没有AliasName, 如下值可能是修改后的UserName?
                    #user.AliasName = userMMKVSetting["87"]

                    user.Headimgurl = userMMKVSetting["headimgurl"]
                    user.HeadImgUrlHD = userMMKVSetting["headhdimgurl"]
                */
            }
        }

        string localHeadImg;
        if (iosArchives.getAbsolutePathByRelativePath("Documents/" + user.second.UserID() + "/lastHeadImage", localHeadImg))
        {
            user.second.setLocalHeadImg(localHeadImg);
        }
        else
        {
            user.second.setLocalHeadImg(LOCAL_DEFAULT_HEAD_IMAGE);
        }
    }
}

void IOSBackupParser::loadUserFriends(WeChatLoginUser& user)
{
    unordered_map<string, WeChatFriend> friends;
    loadUserFriendsFromSessionDB(user.UserID(), friends);
    loadUserFriendsFromMessageDB(user.UserID(), friends);
    loadUserFriendsFromContactDB(user.UserID(), friends);

    user.setFriends(std::move(friends));
    updateLoginUserRecord(user);
}

// Documents/{userID}/session/session.db
static vector<details::SessionRecord> getSessionRecordsByUserID(const string& userID)
{
    vector<details::SessionRecord> records;
    string sessionsDBPath;
    if (iosArchives.getAbsolutePathByRelativePath("Documents/" + userID + "/session/session.db", sessionsDBPath))
    {
        records = details::getSessionRecordsFromDB(sessionsDBPath);
    }
    return records;
}

static vector<details::FriendRecord> getFriendRecordsFromDBByNames(const string& userID, const vector<string>& friendNames)
{
    string contactDBPath;
    if (iosArchives.getAbsolutePathByRelativePath("Documents/" + userID + "/DB/WCDB_Contact.sqlite", contactDBPath))
    {
        return details::getFriendRecordsByNames(contactDBPath, friendNames);
    }
    return {};
}

static bool getFriendRecordFromDBByName(const string& userID, const string& friendName, details::FriendRecord& record)
{
    string contactDBPath;
    if (iosArchives.getAbsolutePathByRelativePath("Documents/" + userID + "/DB/WCDB_Contact.sqlite", contactDBPath))
    {
        return details::getFriendRecordByName(contactDBPath, friendName, record);
    }
    return false;
}

void IOSBackupParser::loadUserFriendsFromSessionDB(const string& userID, unordered_map<string, WeChatFriend>& friends)
{
    vector<details::SessionRecord> records = getSessionRecordsByUserID(userID);
    for (auto& record : records)
    {
        // ConStrRes1: /session/data/c1/96266f837d14e0b693f961bee37b66, 
        // in case ConStrRes1 is none, construct that path by userID = md5(UsrName)
        string friendID = md5(record.userName);

        WeChatFriend& afriend = friends[friendID];
        afriend.setUserID(friendID);
        afriend.setUserName(record.userName);
        afriend.setBeginTime(record.createTime);
        afriend.setRecordCount(0);

        string conStrRes1 = record.conStrRes1;
        if (conStrRes1.empty())
        {
            conStrRes1 = "/session/data/" + friendID.substr(0, 2) + "/" + friendID.substr(2);
        }

        string content;
        if (iosArchives.getFileContentByRelativePath("Documents/" + userID + conStrRes1, content))
        {
            unordered_map<string, string> sessionInfo = Protobuf::toMap(content);
            afriend.setNickName(sessionInfo.count("1.1.4.0") == 0 ? "" : sessionInfo["1.1.4.0"]);
            afriend.setAliasName(sessionInfo.count("1.1.6.0") == 0 ? "" : sessionInfo["1.1.6.0"]);
            afriend.setHeadImgUrl(sessionInfo.count("1.1.14.0") == 0 ? "" : sessionInfo["1.1.14.0"]);
            afriend.setHeadImgUrlHD(sessionInfo.count("1.1.15.0") == 0 ? "" : sessionInfo["1.1.15.0"]);
            afriend.setRecordCount(std::stoi(sessionInfo["2.2.0"]));
            afriend.setLastTime(std::stoi(sessionInfo["2.7.0"]));

            afriend.setCountry(sessionInfo.count("1.6.0") == 0 ? "" : sessionInfo["1.6.0"]);
            afriend.setProvince(sessionInfo.count("1.7.0") == 0 ? "" : sessionInfo["1.7.0"]);
            afriend.setCity(sessionInfo.count("1.8.0") == 0 ? "" : sessionInfo["1.8.0"]);
        }
        else
        {
            details::FriendRecord record;
            if (getFriendRecordFromDBByName(userID, afriend.UserName(), record))
            {
                auto remarks = Protobuf::toMap(record.dbContactRemark);
                afriend.setNickName(remarks.count("1") == 0 ? "" : remarks["1"]);
                afriend.setAliasName(remarks.count("3") == 0 ? "" : remarks["3"]);
            }
            else
            {
                std::cout << afriend.DisplayName() << std::endl;
            }
        }

        string localHeadImg;
        if (iosArchives.getAbsolutePathByRelativePath("share/" + userID + "/session/headImg/" + afriend.UserID() + ".pic", localHeadImg))
        {
            afriend.setLocalHeadImg(localHeadImg);
        }
        else
        {
            afriend.setLocalHeadImg(LOCAL_DEFAULT_HEAD_IMAGE);
        }
    }
}

static vector<string> getMessageTableNamesForUserID(const string& path, const string& userID)
{
    auto records = iosArchives.getManifestRecordsByDomainAndStartEndWith(TENCENT_MANIFEST_DOMAIN, "Documents/" + userID + "/DB/message_", ".sqlite");

    vector<string> dbPaths;
    for (auto record : records)
    {
        dbPaths.push_back(iosArchives.getAbsolutePathByManifestRecord(record));
    }

    string mmSqlitePath;
    if (iosArchives.getAbsolutePathByRelativePath("Documents/" + userID + "/DB/MM.sqlite", mmSqlitePath))
    {
        dbPaths.push_back(mmSqlitePath);
    }

    return dbPaths;
}

void IOSBackupParser::loadUserFriendsFromMessageDB(const string& userID, unordered_map<string, WeChatFriend>& friends)
{
    int firstRecordDate = INT_MAX, lastRecordDate = INT_MIN, total = 0;
    vector<string> dbPaths = getMessageTableNamesForUserID(backupPath, userID);
    for (auto path : dbPaths)
    {
        vector<string> sessionIDs = details::getSessionIDsFromDB(path);
        for (auto sessionID : sessionIDs)
        {
            bool exist = friends.count(sessionID);
            WeChatFriend& afriend = friends[sessionID];
            afriend.setUserID(sessionID);

            auto infos = details::getChatRecordInfoByFriend(path, sessionID);
            if (!infos.empty())
            {
                afriend.appendDbPath(path);
                afriend.setRecordCount(infos[0]);
                afriend.setBeginTime(infos[1]);
                afriend.setLastTime(infos[2]);
            }
            if (!exist)
            {
                afriend.setLocalHeadImg(LOCAL_DEFAULT_HEAD_IMAGE);
            }
        }
    }
}

void IOSBackupParser::loadUserFriendsFromContactDB(const string& userID, unordered_map<string, WeChatFriend>& friends)
{
    for (auto& iter : friends)
    {
        WeChatFriend& f = iter.second;

        details::FriendRecord record;
        if (getFriendRecordFromDBByName(userID, f.UserName(), record))
        {
            unordered_map<string, string> profile = Protobuf::toMap(record.dbContactProfile);
            unordered_map<string, string> remark = Protobuf::toMap(record.dbContactRemark);

            if (f.UserName().empty())
            {
                f.setUserName(record.userName);
            }
            if (f.NickName().empty())
            {
                f.setNickName(remark["1.0"]);
            }
            if (f.Country().empty())
            {
                f.setCountry(profile["2.0"]);
            }
            if (f.Province().empty())
            {
                f.setProvince(profile["3.0"]);
            }
            if (f.City().empty())
            {
                f.setCity(profile["4.0"]);
            }
            f.setSignature(profile["5.0"]);
        }
    }
}

void IOSBackupParser::loadGroupMembers(const WeChatLoginUser& user, WeChatFriend& afriend)
{
    details::FriendRecord record;
    if (getFriendRecordFromDBByName(user.UserID(), afriend.UserName(), record))
    {
        unordered_map<string, string> roomData = Protobuf::toMap(record.dbContactChatRoom);
        if (roomData.count("1.0"))
        {
            auto memberUserNames = Utils::split(roomData["1.0"], ";");
            auto records = getFriendRecordsFromDBByNames(user.UserID(), memberUserNames);

            for (auto& r : records)
            {
                string memberID = md5(r.userName);
                WeChatFriend& member = afriend.getMember(memberID);
                member.setUserName(r.userName);

                auto remarks = Protobuf::toMap(r.dbContactRemark);
                member.setNickName(remarks.count("1.0") == 0 ? "" : remarks["1.0"]);
                member.setAliasName(remarks.count("3.0") == 0 ? "" : remarks["3.0"]);

                string localHeadImg;
                if (iosArchives.getAbsolutePathByRelativePath("share/" + user.UserID() + "/session/headImg/" + member.UserID() + ".pic", localHeadImg))
                {
                    member.setLocalHeadImg(localHeadImg);
                }
                else
                {
                    member.setLocalHeadImg(LOCAL_DEFAULT_HEAD_IMAGE);
                }

                auto headImage =  Protobuf::toMap(r.dbContactHeadImage);
                member.setHeadImgUrl(headImage.count("2.0") == 0 ? "" : headImage["2.0"]);
                member.setHeadImgUrlHD(headImage.count("3.0") == 0 ? "" : headImage["3.0"]);
            }
        }
    }
}

void IOSBackupParser::loadGroupMember(const WeChatLoginUser& user, WeChatFriend& afriend, const string& memberName)
{
    details::FriendRecord record;
    if (getFriendRecordFromDBByName(user.UserID(), memberName, record))
    {
        string memberID = md5(record.userName);
        WeChatFriend& member = afriend.getMember(memberID);
        member.setUserName(record.userName);

        auto remarks = Protobuf::toMap(record.dbContactRemark);
        member.setNickName(remarks.count("1.0") == 0 ? "" : remarks["1.0"]);
        member.setAliasName(remarks.count("3.0") == 0 ? "" : remarks["3.0"]);

        string localHeadImg;
        if (iosArchives.getAbsolutePathByRelativePath("share/" + user.UserID() + "/session/headImg/" + member.UserID() + ".pic", localHeadImg))
        {
            member.setLocalHeadImg(localHeadImg);
        }
        else
        {
            member.setLocalHeadImg(LOCAL_DEFAULT_HEAD_IMAGE);
        }

        auto headImage =  Protobuf::toMap(record.dbContactHeadImage);
        member.setHeadImgUrl(headImage.count("2.0") == 0 ? "" : headImage["2.0"]);
        member.setHeadImgUrlHD(headImage.count("3.0") == 0 ? "" : headImage["3.0"]);
    }
}

vector<WeChatMessage> IOSBackupParser::loadFriendMessages(const WeChatLoginUser& user, WeChatFriend& afriend, int page, int countPerPage)
{
    // itune backup have only 1 db
    if (!afriend.DbPaths().empty())
    {
        auto records = details::getChatRecordsByFriend(afriend.DbPaths()[0], afriend.UserID(), page, countPerPage);

        IOSMessageParser        messageParser(user, afriend, this, iosArchives);
        vector<WeChatMessage>   messages(records.size());
        for (int i = 0; i < messages.size(); ++i)
        {
            messages[i] = messageParser.parse("", records[i].createTime, records[i].message, records[i].des == 0, records[i].type, records[i].mesLocalID);
        }
        return messages;
    }
    return {};
}

string IOSBackupParser::loadUserHeadImgData(const model::WeChatLoginUser& user, const model::WeChatUser& userOrFriend)
{
    // download from HeadImgUrl or HeadImgUrlHD
    return userOrFriend.LocalHeadImg();
}

string IOSBackupParser::loadUserAudioData(const model::WeChatLoginUser& user, const model::WeChatFriend& afriend, const model::WeChatMessage& message)
{
    // do nothing, can't go here
    return "";
}

std::string IOSBackupParser::loadMsgImgData(const std::string& fileName)
{
    // do nothing, can't go here
    return "";
}