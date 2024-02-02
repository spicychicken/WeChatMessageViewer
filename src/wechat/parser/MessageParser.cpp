#include "MessageParser.h"

#include "functions/Utils.h"

#include <iostream>
#include <tinyxml2.h>

using std::string;
using std::vector;
using namespace tinyxml2;

using wechat::parser::MessageParser;
using wechat::parser::BackupFileParser;
using wechat::model::WeChatLoginUser;
using wechat::model::WeChatFriend;
using wechat::model::WeChatMessage;
using wechat::model::ChatMessageType;
using wechat::model::WeChatUser;


WeChatFriend    defaultSender;

static const WeChatUser* getSenderByName(const WeChatLoginUser& user, WeChatFriend& afriend, BackupFileParser* backupParser, const string& senderName)
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

MessageParser::MessageParser(const WeChatLoginUser& u, WeChatFriend& f, BackupFileParser* p)
                    : user(u), afriend(f), backupParser(p)
{
}

WeChatMessage MessageParser::parse(int createTime, const string& message, int des, int type, const string& resID) const
{
    WeChatMessage msg;
    parseBasic(msg, createTime, message, des, type, resID);
    parseByType(msg);
    return msg;
}

void MessageParser::parseBasic(WeChatMessage& msg, int createTime, const std::string& message, int des, int type, const std::string& resID) const
{
    msg.setType(type);
    msg.setTime(createTime);
    msg.setContent(message);
    msg.setResourceID(resID);

    msg.setSender(&defaultSender);

    if (des == 0)
    {
        msg.setSender(&user);
    }
    else
    {
        if (afriend.Type() == wechat::model::UserType::UserType_Group)
        {
            size_t pos = message.find(":\n");
            if (pos != string::npos)
            {
                string senderName = message.substr(0, pos);
                msg.setSender(getSenderByName(user, afriend, backupParser, senderName));
                msg.setContent(message.substr(pos + 2));
            }
        }
        else
        {
            msg.setSender(&afriend);
        }
    }
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

void MessageParser::parseByText(model::WeChatMessage& msg) const
{

}

void MessageParser::parseByImage(model::WeChatMessage& msg) const
{

}

void MessageParser::parseByAudio(model::WeChatMessage& msg) const
{

}

static string getXmlAttributeByPath(const string& xmlContent, const string& path, const string& attr)
{
    string attrValue;
    XMLDocument doc;
    if (doc.Parse(xmlContent.c_str()) == XML_SUCCESS)
    {
        vector<string> paths = Utils::split(path, "/");

        XMLElement* element = doc.RootElement();
        for (int i = 1; i < paths.size() && element; ++i)
        {
            element = element->FirstChildElement(paths[i].c_str());
        }

        if (element)
        {
            const XMLAttribute* attrNode = element->FindAttribute(attr.c_str());
            if (attrNode)
            {
                attrValue.assign(attrNode->Value());
            }
        }
    }
    return attrValue;
}

static string getXmlNodeByPath(const string& xmlContent, const string& path)
{
    string value;
    XMLDocument doc;
    if (doc.Parse(xmlContent.c_str()) == XML_SUCCESS)
    {
        vector<string> paths = Utils::split(path, "/");

        XMLElement* element = doc.RootElement();
        for (int i = 1; i < paths.size() && element; ++i)
        {
            element = element->FirstChildElement(paths[i].c_str());
        }

        if (element && element->FirstChild())
        {
            value.assign(element->FirstChild()->Value());
        }
    }
    return value;
}

void MessageParser::parseByVideo(model::WeChatMessage& msg) const
{
    string senderName = getXmlAttributeByPath(msg.getContent(), "/msg/videomsg", "fromusername");
    if (!senderName.empty())
    {
        msg.setSender(getSenderByName(user, afriend, backupParser, senderName));
    }
}

void MessageParser::parseByEmoticon(model::WeChatMessage& msg) const
{

}

void MessageParser::parseByAppMsg(model::WeChatMessage& msg) const
{
    string senderName = getXmlNodeByPath(msg.getContent(), "/msg/fromusername");
    if (!senderName.empty())
    {
        msg.setSender(getSenderByName(user, afriend, backupParser, senderName));
    }

    string title = getXmlNodeByPath(msg.getContent(), "/msg/appmsg/title");
    string url = getXmlNodeByPath(msg.getContent(), "/msg/appmsg/des");
    string thumburl = getXmlNodeByPath(msg.getContent(), "/msg/appmsg/thumburl");
    
    msg.setContent(title);
    msg.setSrc(url);
    msg.setThumb(thumburl);
}

void MessageParser::parseBySystem(model::WeChatMessage& msg) const
{
    string oldC = msg.getContent(), newC = "";
    if (Utils::startsWith(oldC, "<sysmsg"))
    {
        auto sysMsgType = getXmlAttributeByPath(oldC, "/sysmsg", "type");
        if (sysMsgType == "sysmsgtemplate")
        {
            auto templateType = getXmlAttributeByPath(oldC, "/sysmsg/sysmsgtemplate/content_template", "type");
        }
        msg.setContent("-----------------------------");
    }
    else
    {
        newC = Utils::removeHtmlTags(oldC);
    }
    msg.setContent(newC);
}