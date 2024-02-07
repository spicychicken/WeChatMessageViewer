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

    std::string UserID() const          { return userID; }
    std::string UserName() const        { return userName; }
    std::string NickName() const        { return nickName; }
    std::string AliasName() const       { return aliasName; }
    std::string LocalHeadImg() const    { return localHeadImg; }
    std::string HeadImgUrl() const      { return headImgUrl; }
    std::string HeadImgUrlHD() const    { return headImgUrlHD; }

    std::string DisplayName() const
    {
        if (aliasName != "")
        {
            return aliasName;
        }
        else if (nickName != "")
        {
            return nickName;
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

    model::WeChatFriend& WeChatFriend::getMember(const std::string& memberID);
    const model::WeChatFriend& getMember(const std::string& memberID) const;

    std::string DbPath() const                      { return dbPath; }
    void setDbPath(const std::string& dbPath)       { this->dbPath = dbPath; }

private:
    std::string                                     dbPath;
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

private:
    std::vector<model::WeChatFriend>            friends;
    std::unordered_map<std::string, int>        idToIndexs;
};

class WeChatBackup
{
public:
    void setITuneVersion(const std::string& version)                  { iTuneVersion = version; }
    void setProductVersion(const std::string& version)                { productVersion = version; }
    void setLastBackupDate(const std::string& date)                   { lastBackupDate = date; }

    void setLoginUsers(std::unordered_map<std::string, model::WeChatLoginUser>&& loginUsers)    { this->loginUsers = loginUsers; }

    bool loginUserExistByID(const std::string& userID);
    model::WeChatLoginUser& getLoginUserByID(const std::string& userID);

    const std::unordered_map<std::string, model::WeChatLoginUser> getLoginUsers() const         { return loginUsers; }
    std::string getITuneVersion() const                 { return iTuneVersion; }
    std::string getProductVersion() const               { return productVersion; }
    std::string getLastBackupDate() const               { return lastBackupDate; }

private:
    std::unordered_map<std::string, model::WeChatLoginUser>     loginUsers;
    std::string                     iTuneVersion;
    std::string                     productVersion;
    std::string                     lastBackupDate;
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
    const WeChatUser* getSender() const               { return sender; }
    // std::string getSenderName() const;
    std::string getContent() const                    { return content; }
    int getTime() const                               { return msgTime; }
    ChatMessageType getType() const                   { return type; }
    std::string getResourceID() const                 { return resourceID; }

    std::string getSrc() const                        { return src; }
    std::string getThumb() const                      { return thumb; }

public:
    void setTime(int t)                               { this->msgTime = t; }
    void setSender(const WeChatUser* s)               { this->sender = s; }
    // void setSender(const std::string& name)           { this->senderName = name; }
    void setContent(const std::string& c)             { this->content = c; }
    void setType(int type);
    void setResourceID(const std::string& resourceID) { this->resourceID = resourceID; }

    void setSrc(const std::string& src)               { this->src = src; }
    void setThumb(const std::string& thumb)           { this->thumb = thumb; }

private:
    ChatMessageType                 type;
    const WeChatUser*               sender = nullptr;
    // std::string                     senderName;
    std::string                     content;
    int                             msgTime;
    std::string                     resourceID;

    // 
    std::string                     src;
    std::string                     thumb;
};

}   // wechat::model

#endif // WECHATMODEL_H
