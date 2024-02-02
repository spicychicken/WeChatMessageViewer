#include "details.h"

#include "sqlite3cpp/Sqlite3DBReader.h"
#include "md5/md5.h"

using std::string;
using std::vector;
using std::tuple;
using std::unordered_map;

using namespace wechat::parser::ios;

vector<details::SessionRecord> details::getSessionRecordsFromDB(const string& dbPath)
{
    const string countSql = "SELECT count(*) FROM SessionAbstract";
    const string querySql = "SELECT ConStrRes1,CreateTime,unreadcount,UsrName FROM SessionAbstract";

    int count = sqlitedb::queryCount(dbPath, countSql);

    if (count != 0)
    {
        sqlitedb::StmtReader    reader;
        if (sqlitedb::queryData(dbPath, querySql, reader))
        {
            vector<details::SessionRecord>  records(count);
            for (int i = 0; i < count && reader.next(); ++i)
            {
                records[i].conStrRes1   = reader.readString(0);
                records[i].createTime   = reader.readInt(1);
                records[i].unReadCount  = reader.readInt(2);
                records[i].userName     = reader.readString(3);
            }
            return records;
        }
    }
    return {};
}

string details::getNSObjectsByKey(const unordered_map<string, string>& setting, const std::string& value)
{
    for (int i = 0; i < 50; ++i)
    {
        auto key = "$objects.1.new_dicsetting.NS.keys." + std::to_string(i);
        if (setting.count(key) != 0)
        {
            if (setting.at(key) == value)
            {
                return setting.at("$objects.1.new_dicsetting.NS.objects." + std::to_string(i));
            }
        }
        else
        {
            return "";
        }
    }
    return "";
}

vector<string> details::getSessionIDsFromDB(const string& dbPath)
{
    sqlitedb::StmtReader    reader;
    if (sqlitedb::queryData(dbPath, "SELECT name FROM sqlite_master WHERE type='table' and name like 'Chat\\_%%' escape '\\'", reader))
    {
        vector<string> sessionIDs;
        while (reader.next())
        {
            sessionIDs.push_back(reader.readString(0).substr(5));
        }
        return sessionIDs;
    }
    return {};
}

bool details::getFriendRecordByName(const string& dbPath, const string& friendName, details::FriendRecord& record)
{
    string querySql = "SELECT userName,dbContactChatRoom,dbContactLocal,dbContactRemark,dbContactProfile,dbContactSocial,type,dbContactHeadImage FROM Friend where userName = '" + friendName + "'";

    sqlitedb::StmtReader    reader;
    if (sqlitedb::queryData(dbPath, querySql, reader))
    {
        if (reader.next())
        {      
            record.userName             = reader.readString(0);
            record.dbContactChatRoom    = reader.readBlob(1);
            record.dbContactLocal       = reader.readBlob(2);
            record.dbContactRemark      = reader.readBlob(3);
            record.dbContactProfile     = reader.readBlob(4);
            record.dbContactSocial      = reader.readBlob(5);
            record.type                 = reader.readInt(6);
            record.dbContactHeadImage   = reader.readBlob(7);

            return true;
        }
    }
    return false;
}

vector<details::FriendRecord> details::getFriendRecordsByNames(const string& dbPath, const vector<string>& friendNames)
{
    string querySql = "SELECT userName,dbContactChatRoom,dbContactLocal,dbContactRemark,dbContactProfile,dbContactSocial,type,dbContactHeadImage FROM Friend where userName in (";
    for (auto& name : friendNames)
    {
        querySql += ("'" + name + "',");
    }
    querySql.replace(querySql.length() - 1, 1, ")");


    sqlitedb::StmtReader    reader;
    if (sqlitedb::queryData(dbPath, querySql, reader))
    {
        vector<details::FriendRecord>   records;
        while (reader.next())
        {
            details::FriendRecord    record;
            record.userName             = reader.readString(0);
            record.dbContactChatRoom    = reader.readBlob(1);
            record.dbContactLocal       = reader.readBlob(2);
            record.dbContactRemark      = reader.readBlob(3);
            record.dbContactProfile     = reader.readBlob(4);
            record.dbContactSocial      = reader.readBlob(5);
            record.type                 = reader.readInt(6);
            record.dbContactHeadImage   = reader.readBlob(7);

            records.push_back(record);
        }
        return records;
    }
    return {};
}

unordered_map<string, details::FriendRecord> details::getFriendRecordsFromDB(const string& dbPath)
{
    string querySql = "SELECT userName,dbContactChatRoom,dbContactLocal,dbContactRemark,dbContactProfile,dbContactSocial,type,dbContactHeadImage FROM Friend";

    unordered_map<string, details::FriendRecord> friends;

    sqlitedb::StmtReader    reader;
    if (sqlitedb::queryData(dbPath, querySql, reader))
    {
        while (reader.next())
        {
            string userName             = reader.readString(0);
            string userID               = md5(userName);

            auto& afriend = friends[userID];
            afriend.userName            = userName;
            afriend.dbContactChatRoom   = reader.readBlob(1);
            afriend.dbContactLocal      = reader.readBlob(2);
            afriend.dbContactRemark     = reader.readBlob(3);
            afriend.dbContactProfile    = reader.readBlob(4);
            afriend.dbContactSocial     = reader.readBlob(5);
            afriend.type                = reader.readInt(6);
            afriend.dbContactHeadImage  = reader.readBlob(7);
        }
    }
    return friends;
}

vector<details::ChatRecord> details::getChatRecordsByFriend(const string& dbPath, const string& friendID, int page, int countPerPage) {
    const string querySql = "select CreateTime,Message,Des,Type,MesLocalID from Chat_" + friendID
        + " ORDER BY CreateTime limit " + std::to_string(countPerPage) + " offset " + std::to_string(countPerPage * (page - 1));

    sqlitedb::StmtReader reader;
    if (sqlitedb::queryData(dbPath, querySql, reader))
    {
        vector<ChatRecord>   messages;
        while (reader.next())
        {
            ChatRecord   msg;
            msg.createTime  = reader.readInt(0);
            msg.message     = reader.readString(1);
            msg.des         = reader.readInt(2);
            msg.type        = reader.readInt(3);
            msg.mesLocalID  = reader.readString(4);
            messages.emplace_back(msg);
        }
        return messages;
    }
    return {};
}

vector<int> details::getChatRecordInfoByFriend(const string& dbPath, const string& friendID)
{
    vector<int>    infos;
    sqlitedb::StmtReader reader;
    if (sqlitedb::queryData(dbPath, "select count(*), MIN(CreateTime), MAX(CreateTime) from Chat_" + friendID, reader))
    {
        if (reader.next())
        {
            infos.push_back(reader.readInt(0));
            infos.push_back(reader.readInt(1));
            infos.push_back(reader.readInt(2));
        }
    }
    return infos;
}