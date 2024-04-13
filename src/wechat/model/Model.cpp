#include "wechat/model/Model.h"

#include <algorithm>
#include <iterator>

using std::vector;
using std::string;
using std::unordered_map;

using wechat::model::UserType;
using wechat::model::WeChatUser;
using wechat::model::WeChatLoginUser;
using wechat::model::WeChatBackup;
using wechat::model::WeChatFriend;
using wechat::model::WeChatMessage;

// newsapp,fmessage,filehelper,weibo,qqmail,fmessage,tmessage,qmessage,qqsync,floatbottle,lbsapp,shakeapp,
// medianote,qqfriend,readerapp,blogapp,facebookapp,masssendapp,meishiapp,feedsapp,voip,blogappweixin,weixin,
// brandsessionholder,weixinreminder,wxid_novlwrv3lqwv11,gh_22b87fa7cb3c,officialaccounts,notification_messages,
// wxid_novlwrv3lqwv11,gh_22b87fa7cb3c,wxitil,userexperience_alarm,notification_messages
bool WeChatUser::isSubscription(const string& userName)
{
    return (userName.length() > 3 && userName.compare(0, 3, "gh_") == 0)
            || (userName.compare("brandsessionholder") == 0)
            || (userName.compare("newsapp") == 0)
            || (userName.compare("weixin") == 0)
            || (userName.compare("notification_messages") == 0)
            || (userName.compare("@publicUser") == 0)
            || (userName.compare("mphelper") == 0)              // 公众平台安全助手
            || (userName.compare("notifymessage") == 0)         // 服务通知
            || (userName.compare("@CustomerService") == 0);     // 小程序客服消息
}

void WeChatLoginUser::setFriends(unordered_map<std::string, model::WeChatFriend>&& friends)
{
    int index = 0;
    this->friends.resize(friends.size());
    for (auto& f : friends)
    {
        this->friends[index] = f.second;
        idToIndexs[f.first] = index++;
    }
}

int WeChatLoginUser::getFriendsCountByType(UserType type)
{
    return std::count_if(friends.begin(), friends.end(), [&type](auto& f) {
        return f.Type() == type;
    });
}

WeChatFriend& WeChatLoginUser::getFriend(const string& friendID)
{
    if (idToIndexs.count(friendID) == 0)
    {
        friends.push_back(WeChatFriend());
        idToIndexs[friendID] = friends.size() - 1;
    }
    return friends[idToIndexs[friendID]];
}

const WeChatFriend& WeChatLoginUser::getFriend(const std::string& friendID) const
{
    return friends.at(idToIndexs.at(friendID));
}

vector<WeChatFriend> WeChatLoginUser::getTopFriends(UserType type, int count, std::function<bool(WeChatFriend&, WeChatFriend&)> func)
{
    std::vector<WeChatFriend>       results;
    std::copy_if(friends.begin(), friends.end(), std::back_inserter(results), [&type](auto& session) {
        return session.Type() == type;
    });

    std::sort(results.begin(), results.end(), func);
    if (results.size() > count)
    {
        results.resize(count);
    }

    return results;
}

WeChatFriend& WeChatFriend::getMember(const string& memberID)
{
    return members[memberID];
}

const WeChatFriend& WeChatFriend::getMember(const string& memberID) const
{
    return members.at(memberID);
}

bool WeChatBackup::loginUserExistByID(const string& userID)
{
    return loginUsers.count(userID) != 0;
}

WeChatLoginUser& WeChatBackup::getLoginUserByID(const string& userID)
{
    return loginUsers[userID];
}

/*
std::string WeChatMessage::getSenderName() const
{
    if (sender)
    {
        return sender->DisplayName();
    }
    return senderName;
}
*/

void WeChatMessage::setType(int type)
{
    switch (type)
    {
        case 1:
            this->type = ChatMessageType::Text;
            break;
        case 3:
            this->type = ChatMessageType::Image;
            break;
        case 34:
            this->type = ChatMessageType::Audio;
            break;
        case 43:
            this->type = ChatMessageType::Video;
            break;
        case 47:
            this->type = ChatMessageType::Emoticon;
            break;
        case 48:
            this->type = ChatMessageType::Location;
            break;
        case 49:
            this->type = ChatMessageType::AppMessage;
            break;
        case 10000:
            this->type = ChatMessageType::System;
            break;
        case 10002:
            this->type = ChatMessageType::System_Recalled;
            break;
        default:
            this->type = ChatMessageType::Other;
    }
}
