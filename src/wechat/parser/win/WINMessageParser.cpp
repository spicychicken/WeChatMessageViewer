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
        string senderName, senderID;
        if (msg.getType() == ChatMessageType::Text)
        {
            auto results = Protobuf::toMap(msg.getExtra());

            // [To-Do] workaround
            senderName = results["3.2.0"];
            if (senderName.substr(0, 11) == "<msgsource>")
            {
                senderName = Utils::getXmlNodeByPath(senderName, "/msgsource/atuserlist");
            }
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
    // do nothing
}

// workaround
void workaroudForThumbSrcRaw(WINBackupArchives& winArchives, WeChatMessage& msg, std::unordered_map<std::string, std::string>& results, const std::string& userName, int start, int end)
{
    for (int i = start; i < end; i++)
    {
        auto key = "3.1." + std::to_string(i);
        if (results.count(key) != 0)
        {
            if (results[key] == "3")
            {
                msg.setMetadata("thumb", winArchives.correctPath(userName, results["3.2." + std::to_string(i)]));
            }
            else if (results[key] == "4")
            {
                msg.setMetadata("src", winArchives.correctPath(userName, results["3.2." + std::to_string(i)]));
            }
            else if (results[key] == "18")
            {
                msg.setMetadata("raw", winArchives.correctPath(userName, results["3.2." + std::to_string(i)]));
            }
        }
    }
}

void WINMessageParser::parseByImage(model::WeChatMessage& msg) const
{
    auto results = Protobuf::toMap(msg.getExtra());
    workaroudForThumbSrcRaw(winArchives, msg, results, user.UserName(), 1, 5);
    /* if (afriend.Type() == wechat::model::UserType::UserType_Group)
    {
        if (results["3.1.2"] == "4")
        {
            msg.setMetadata("thumb", winArchives.correctPath(user.UserName(), results["3.2.3"]));
            msg.setMetadata("src", winArchives.correctPath(user.UserName(), results["3.2.2"]));
        }
        else if (results["3.1.3"] == "4")
        {
            msg.setMetadata("thumb", winArchives.correctPath(user.UserName(), results["3.2.4"]));
            msg.setMetadata("src", winArchives.correctPath(user.UserName(), results["3.2.3"]));
        }
    }
    else
    {
        if (results["3.1.1"] == "3") {
            msg.setMetadata("thumb", winArchives.correctPath(user.UserName(), results["3.2.1"]));
        }
        else if (results["3.1.3"] == "3")
        {
            msg.setMetadata("thumb", winArchives.correctPath(user.UserName(), results["3.2.3"]));
        }
        msg.setMetadata("src", winArchives.correctPath(user.UserName(), results["3.2.2"]));
    }   */
}

void WINMessageParser::parseByAudio(model::WeChatMessage& msg) const
{
    auto results = Protobuf::toMap(msg.getExtra());
    string senderName = results["3.2.0"];

    int seconds = std::stoi(Utils::getXmlAttributeByPath(msg.getContent(), "/msg/voicemsg", "voicelength")) / 1000 + 0.5;
    msg.setMetadata("seconds", std::to_string(seconds));
}

void WINMessageParser::parseByVideo(model::WeChatMessage& msg) const
{
    auto results = Protobuf::toMap(msg.getExtra());
    workaroudForThumbSrcRaw(winArchives, msg, results, user.UserName(), 1, 5);
    /* if (afriend.Type() == wechat::model::UserType::UserType_Group)
    {
        // [To-Do] workaround
        if (results["3.2.1"].substr(0, 11) == "<msgsource>")
        {
            msg.setMetadata("thumb", winArchives.correctPath(user.UserName(), results["3.2.3"]));
            msg.setMetadata("src", winArchives.correctPath(user.UserName(), results["3.2.4"]));
            msg.setMetadata("raw", winArchives.correctPath(user.UserName(), results["3.2.5"]));
        }
        else
        {
            msg.setMetadata("thumb", winArchives.correctPath(user.UserName(), results["3.2.2"]));
            msg.setMetadata("src", winArchives.correctPath(user.UserName(), results["3.2.3"]));
            msg.setMetadata("raw", winArchives.correctPath(user.UserName(), results["3.2.4"]));
        }
    }
    else
    {
        msg.setMetadata("thumb", winArchives.correctPath(user.UserName(), results["3.2.2"]));
        msg.setMetadata("src", winArchives.correctPath(user.UserName(), results["3.2.3"]));
    }   */
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

    msg.setMetadata("thumb", thumburl);
    msg.setMetadata("src", url);

    // [To-Do]
    msg.setContent("[App Message]");
}

void WINMessageParser::parseBySystem(model::WeChatMessage& msg) const
{
    string oldC = msg.getContent();
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
        string newC = Utils::removeHtmlTags(oldC);
        msg.setContent(newC);
    }

    // [To-Do]
    msg.setContent("[System Message]");
}

void WINMessageParser::parseByOther(model::WeChatMessage& msg) const
{
    // [To-Do]
    msg.setContent("[Other]");
}