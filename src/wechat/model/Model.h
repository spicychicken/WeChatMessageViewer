#ifndef WECHATMODEL_H
#define WECHATMODEL_H

#include <string>
#include <functional>
#include <unordered_map>

#include "md5/md5.h"

namespace wechat::model
{

enum class UserType
{
    UserType_LoginUser,
    UserType_Friend,
    UserType_Group,
    UserType_Other,
};

class WeChatUser
{
public:
    static bool isSubscription(const std::string& userName);
    virtual UserType Type() const = 0;

    std::string Country() const     { return country; }
    std::string Province() const    { return province; }
    std::string City() const        { return city; }
    std::string Email() const       { return email; }
    std::string Phone() const       { return phone; }
    std::string Remark() const      { return remark; }
    std::string Signature() const   { return signature; }

    std::string UserID() const          { return userID; }
    std::string UserName() const        { return userName; }
    std::string NickName() const        { return nickName; }
    std::string AliasName() const       { return aliasName; }

    std::string LocalHeadImg() const    { return localHeadImg; }
    std::string HeadImgUrl() const      { return headImgUrl; }
    std::string HeadImgUrlHD() const    { return headImgUrlHD; }

    std::string HeadImg() const
    {
        if (headImgUrlHD != "")
        {
            return headImgUrlHD;
        }
        return headImgUrl;
    }

    std::string DisplayName() const
    {
        if (remark != "")
        {
            return remark;
        }
        else if (nickName != "")
        {
            return nickName;
        }
        else if (aliasName != "")
        {
            return aliasName;
        }
        else if (userName != "")
        {
            return userName;
        }
        return userID;
    }

    int BeginTime() const               { return beginTime; }
    int LastTime() const                { return lastTime; }
    int RecordCount() const             { return recordCount; }

public:
    void setCountry(const std::string& country)     { this->country = country; }
    void setProvince(const std::string& province)   { this->province = province; }
    void setCity(const std::string& city)           { this->city = city; }
    void setEmail(const std::string& email)         { this->email = email; }
    void setPhone(const std::string& phone)         { this->phone = phone; }
    void setRemark(const std::string& remark)       { this->remark = remark; }
    void setSignature(const std::string& signature) { this->signature = signature; }

    void setUserID(const std::string& userID)               { this->userID = userID; }
    void setNickName(const std::string& nickName)           { this->nickName = nickName; }
    void setAliasName(const std::string& aliasName)         { this->aliasName = aliasName; }
    void setLocalHeadImg(const std::string& localHeadImg)   { this->localHeadImg = localHeadImg; }
    void setHeadImgUrl(const std::string& headImgUrl)       { this->headImgUrl = headImgUrl; }
    void setHeadImgUrlHD(const std::string& headImgUrlHD)   { this->headImgUrlHD = headImgUrlHD; }
    void setUserName(const std::string& userName)
    {
        this->userName = userName;
        this->userID = md5(userName);
    }
    void setBeginTime(int beginTime)                        { this->beginTime = beginTime; }
    void setLastTime(int lastTime)                          { this->lastTime = lastTime; }
    void setRecordCount(int recordCount)                    { this->recordCount = recordCount; }

private:
    // basic info
    std::string country;
    std::string province;
    std::string city;
    std::string email;
    std::string phone;
    std::string remark;
    std::string signature;

    std::string userID;
    std::string userName;
    std::string nickName;
    std::string aliasName;
    std::string localHeadImg;
    std::string headImgUrl;
    std::string headImgUrlHD;

    // chat info
    int             beginTime;
    int             lastTime;
    int             recordCount;
};

class WeChatFriend : public WeChatUser
{
public:
    UserType Type() const override
    {
        if (isSubscription(UserName())) {
            return UserType::UserType_Other;
        }
        else if (UserName().find("@chatroom") != std::string::npos)
        {
            return UserType::UserType_Group;
        }
        return UserType::UserType_Friend;
    }

    bool hasMemberData() const                      { return members.size() != 0; }

    bool memberExist(const std::string& memberID) const   { return members.count(memberID); }

    model::WeChatFriend& getMember(const std::string& memberID);
    const model::WeChatFriend& getMember(const std::string& memberID) const;

    // std::string DbPath() const                      { return dbPath; }
    // void setDbPath(const std::string& dbPath)       { this->dbPath = dbPath; }

    std::vector<std::string> DbPaths() const           { return dbPaths; }
    void appendDbPath(const std::string& dbPath)       { dbPaths.push_back(dbPath); }

    std::vector<int> DbCounts() const                  { return dbCounts; }
    void appendDBPathAndCount(const std::string& dbPath, int count)
    {
        dbPaths.push_back(dbPath);
        dbCounts.push_back(count);
    }

private:
    std::vector<int>                                dbCounts;        // used in WIN PC
    std::vector<std::string>                        dbPaths;
    std::unordered_map<std::string, WeChatFriend>   members;    // group members
};

class WeChatLoginUser : public WeChatUser
{
public:
    UserType Type() const override           { return UserType::UserType_LoginUser; }
    void setFriends(std::unordered_map<std::string, model::WeChatFriend>&& friends);

    bool hasFriendData() const               { return friends.size() != 0; }

    bool friendExist(const std::string& friendID) const   { return idToIndexs.count(friendID); }

    int getFriendsCountByType(UserType type);
    model::WeChatFriend& getFriend(const std::string& friendID);
    const model::WeChatFriend& getFriend(const std::string& friendID) const;

    std::vector<model::WeChatFriend> getTopFriends(UserType type, int count, std::function<bool(WeChatFriend&, WeChatFriend&)> func);

    const std::vector<model::WeChatFriend>& getFriends() const      { return friends; }

    void setSecretKey(const std::string& secretKey)     { this->secretKey = secretKey; }
    const std::string getSecretKey() const              { return secretKey; }

    void cleanup();

private:
    std::vector<model::WeChatFriend>            friends;
    std::unordered_map<std::string, int>        idToIndexs;
    std::string                                 secretKey;
};

enum class BackupType
{
    BackupType_IOS = 0,
    BackupType_WIN,
    BackupType_UNKNOWN,
};

class WeChatBackup
{
public:
    void setBackupType(BackupType type)                               { backupType = type; }
    void setBackupPath(const std::string& path)                       { backupPath = path; }

    void setLoginUsers(std::unordered_map<std::string, model::WeChatLoginUser>&& loginUsers)    { this->loginUsers = loginUsers; }

    bool loginUserExistByID(const std::string& userID);
    model::WeChatLoginUser& getLoginUserByID(const std::string& userID);

    const std::unordered_map<std::string, model::WeChatLoginUser> getLoginUsers() const         { return loginUsers; }

    BackupType getBackupType() const                    { return backupType; }

    bool metadataExist(const std::string& tag)                          { return metadatas.count(tag) != 0; }
    void setMetadata(const std::string& tag, const std::string& data)   { metadatas[tag] = data; }
    std::string getMetadata(const std::string& tag)                     { return metadatas[tag]; }

    void cleanup();

private:
    std::unordered_map<std::string, model::WeChatLoginUser>     loginUsers;

    // iOS: iTuneVersion, productVersion, lastBackupDate
    std::unordered_map<std::string, std::string>        metadatas;

    std::string                     backupPath;
    BackupType                      backupType;
};

enum class ChatMessageType
{
    Text = 1,
    Image = 3,
    Audio = 34,
    Video = 43,
    Emoticon = 47,
    Location = 48,
    AppMessage = 49,
    System = 10000,
    System_Recalled = 10002,
    Other,
};

class WeChatMessage
{
public:
    const WeChatUser* getSender() const                             { return sender; }
    std::string getContent() const                                  { return content; }
    std::string getMsgSvrID() const                                 { return msgSvrID; }
    int getTime() const                                             { return msgTime; }
    ChatMessageType getType() const                                 { return type; }
    std::string getExtra() const                                    { return extra; }
    std::string getMetadata(const std::string& tag) const           { return metadatas.at(tag); }

    const std::unordered_map<std::string, std::string>& getMetadatas() const   { return metadatas; }

public:
    void setTime(int t)                               { this->msgTime = t; }
    void setSender(const WeChatUser* s)               { this->sender = s; }
    void setContent(const std::string& c)             { this->content = c; }
    void setMsgSvrID(const std::string& id)           { this->msgSvrID = id; }
    void setType(int type);
    void setExtra(const std::string& extra)           { this->extra = extra; }

    void setMetadata(const std::string& tag, const std::string& data)   { metadatas[tag] = data; }

private:
    ChatMessageType                 type;
    const WeChatUser*               sender = nullptr;
    std::string                     msgSvrID;
    std::string                     content;
    int                             msgTime;
    std::string                     extra;

    // dbPath, src, thumb
    std::unordered_map<std::string, std::string>        metadatas;
};

}   // wechat::model

#endif // WECHATMODEL_H
