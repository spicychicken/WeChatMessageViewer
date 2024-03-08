#include "MessageParser.h"

#include "functions/Utils.h"

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

WeChatMessage MessageParser::parse(int createTime, const string& content, bool isSender, int type, const string& extra) const
{
    WeChatMessage msg;
    parseBasic(msg, createTime, content, isSender, type, extra);
    parseSender(msg);
    parseByType(msg);
    return msg;
}

void MessageParser::parseBasic(WeChatMessage& msg, int createTime, const std::string& message, bool isSender, int type, const std::string& extra) const
{
    msg.setType(type);
    msg.setTime(createTime);
    msg.setContent(message);
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
            break;
    }
}

void MessageParser::parseSender(model::WeChatMessage& msg) const
{

}

void MessageParser::parseByText(model::WeChatMessage& msg) const
{

}

void MessageParser::parseByImage(model::WeChatMessage& msg) const
{

}

void MessageParser::parseByAudio(model::WeChatMessage& msg) const
{

}

void MessageParser::parseByVideo(model::WeChatMessage& msg) const
{
    string senderName = Utils::getXmlAttributeByPath(msg.getContent(), "/msg/videomsg", "fromusername");
    if (!senderName.empty())
    {
        msg.setSender(getSenderByName(senderName));
    }
}

void MessageParser::parseByEmoticon(model::WeChatMessage& msg) const
{

}

void MessageParser::parseByAppMsg(model::WeChatMessage& msg) const
{
    string senderName = Utils::getXmlNodeByPath(msg.getContent(), "/msg/fromusername");
    if (!senderName.empty())
    {
        msg.setSender(getSenderByName(senderName));
    }

    string title = Utils::getXmlNodeByPath(msg.getContent(), "/msg/appmsg/title");
    string url = Utils::getXmlNodeByPath(msg.getContent(), "/msg/appmsg/des");
    string thumburl = Utils::getXmlNodeByPath(msg.getContent(), "/msg/appmsg/thumburl");
    
    msg.setContent(title);
    msg.setSrc(url);
    msg.setThumb(thumburl);
}

void MessageParser::parseBySystem(model::WeChatMessage& msg) const
{
    string oldC = msg.getContent(), newC = "";
    if (Utils::startsWith(oldC, "<sysmsg"))
    {
        auto sysMsgType = Utils::getXmlAttributeByPath(oldC, "/sysmsg", "type");
        if (sysMsgType == "sysmsgtemplate")
        {
            auto templateType = Utils::getXmlAttributeByPath(oldC, "/sysmsg/sysmsgtemplate/content_template", "type");
        }
        msg.setContent("-----------------------------");
    }
    else
    {
        newC = Utils::removeHtmlTags(oldC);
    }
    msg.setContent(newC);
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