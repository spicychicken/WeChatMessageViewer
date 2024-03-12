#include "WINMessageParser.h"

#include <iostream>

#include <string>
#include <regex>
#include <vector>

#include "functions/Utils.h"
#include "functions/Protobuf.h"

using std::vector;
using std::string;

using wechat::model::WeChatLoginUser;
using wechat::model::WeChatFriend;
using wechat::model::WeChatMessage;
using wechat::model::ChatMessageType;

using namespace wechat::parser::win;

WINMessageParser::WINMessageParser(const WeChatLoginUser& u, WeChatFriend& f, 
    BackupFileParser* p, WINBackupArchives& archives) : MessageParser(u, f, p), winArchives(archives)
{
    usedUserNames = winArchives.listUsedUserNameFor(u.UserName());
}

void WINMessageParser::parseSender(model::WeChatMessage& msg, bool isSender) const
{
    if (afriend.Type() == wechat::model::UserType::UserType_Group && !isSender)
    {
        string senderName;
        if (msg.getType() == ChatMessageType::Text)
        {
            auto results = Protobuf::toMap(msg.getExtra());
            senderName = results["3.2.0"];
        }
        else if (msg.getType() == ChatMessageType::Audio)
        {
            senderName = Utils::getXmlAttributeByPath(msg.getContent(), "/msg/voicemsg", "fromusername");
        }
        else if (msg.getType() == ChatMessageType::Video)
        {
            senderName = Utils::getXmlAttributeByPath(msg.getContent(), "/msg/videomsg", "fromusername");
        }
        else if (msg.getType() == ChatMessageType::Image)
        {
            auto results = Protobuf::toMap(msg.getExtra());
            senderName = results["3.2.0"];
        }

        if (!senderName.empty())
        {
            msg.setSender(getSenderByName(senderName));
        }
    }
}

void WINMessageParser::parseByText(model::WeChatMessage& msg) const
{
}

void WINMessageParser::parseByImage(model::WeChatMessage& msg) const
{
    auto results = Protobuf::toMap(msg.getExtra());
    if (afriend.Type() == wechat::model::UserType::UserType_Group)
    {
        msg.setThumb(winArchives.correctPath(user.UserName(), results["3.2.4"]));
        msg.setSrc(winArchives.correctPath(user.UserName(), results["3.2.3"]));
    }
    else
    {
        msg.setThumb(winArchives.correctPath(user.UserName(), results["3.2.2"]));
        msg.setSrc(winArchives.correctPath(user.UserName(), results["3.2.1"]));
    }
}

void WINMessageParser::parseByAudio(model::WeChatMessage& msg) const
{
    auto results = Protobuf::toMap(msg.getExtra());
    string senderName = results["3.2.0"];
}

void WINMessageParser::parseByVideo(model::WeChatMessage& msg) const
{
    auto results = Protobuf::toMap(msg.getExtra());
    if (afriend.Type() == wechat::model::UserType::UserType_Group)
    {
        msg.setThumb(winArchives.correctPath(user.UserName(), results["3.2.3"]));
        if (results.count("3.2.5") && !results["3.2.5"].empty())
        {
            msg.setSrc(winArchives.correctPath(user.UserName(), results["3.2.5"]));
        }
        else
        {
            msg.setSrc(winArchives.correctPath(user.UserName(), results["3.2.4"]));
        }
    }
    else
    {
        msg.setThumb(winArchives.correctPath(user.UserName(), results["3.2.2"]));
        msg.setSrc(winArchives.correctPath(user.UserName(), results["3.2.3"]));
    }
}

void WINMessageParser::parseByEmoticon(model::WeChatMessage& msg) const
{
    msg.setContent("[Emoticon]");
}

void WINMessageParser::parseByAppMsg(model::WeChatMessage& msg) const
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

void WINMessageParser::parseBySystem(model::WeChatMessage& msg) const
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

void WINMessageParser::parseByOther(model::WeChatMessage& msg) const
{
}