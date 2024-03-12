#include "MessageParser.h"

using std::string;
using std::vector;

using wechat::parser::MessageParser;
using wechat::parser::BackupFileParser;
using wechat::model::WeChatLoginUser;
using wechat::model::WeChatFriend;
using wechat::model::WeChatMessage;
using wechat::model::ChatMessageType;
using wechat::model::WeChatUser;

MessageParser::MessageParser(const WeChatLoginUser& u, WeChatFriend& f, BackupFileParser* p)
                    : user(u), afriend(f), backupParser(p)
{
}

WeChatMessage MessageParser::parse(const string& msgSvrID, int createTime, const string& content, bool isSender, int type, const string& extra) const
{
    WeChatMessage msg;
    parseBasic(msg, msgSvrID, createTime, content, isSender, type, extra);
    parseSender(msg, isSender);
    parseByType(msg);
    return msg;
}

void MessageParser::parseBasic(WeChatMessage& msg, const std::string& msgSvrID, int createTime, const std::string& message, bool isSender, int type, const std::string& extra) const
{
    msg.setType(type);
    msg.setTime(createTime);
    msg.setContent(message);
    msg.setMsgSvrID(msgSvrID);
    if (isSender) {
        msg.setSender(&user);
    }
    else {
        msg.setSender(&afriend);
    }
    msg.setExtra(extra);
}

void MessageParser::parseByType(WeChatMessage& msg) const
{
    switch (msg.getType())
    {
        case ChatMessageType::Text:
            parseByText(msg);
            break;
        case ChatMessageType::Image:
            parseByImage(msg);
            break;
        case ChatMessageType::Audio:
            parseByAudio(msg);
            break;
        case ChatMessageType::Video:
            parseByVideo(msg);
            break;
        case ChatMessageType::Emoticon:
            parseByEmoticon(msg);
            break;
        case ChatMessageType::AppMessage:
            parseByAppMsg(msg);
            break;
        case ChatMessageType::System:
        case ChatMessageType::System_Recalled:
            parseBySystem(msg);
            break;
        default:
            parseByOther(msg);
            break;
    }
}

const WeChatUser* MessageParser::getSenderByName(const string& senderName) const
{
    string senderID = md5(senderName);
    if (user.UserID() == senderID)
    {
        return &user;
    }
    else if (afriend.UserID() == senderID)
    {
        return &afriend;
    }
    else if (afriend.memberExist(senderID))
    {
        return &afriend.getMember(senderID);
    }
    else if (user.friendExist(senderID))
    {
        return &user.getFriend(senderID);
    }
    else
    {
        backupParser->loadGroupMember(user, afriend, senderName);
        auto sender = &afriend.getMember(senderID);
        sender->setUserName(senderName);
        return sender;
    }
}