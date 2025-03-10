#include "IOSMessageParser.h"

#include <string>

#include "functions/Utils.h"

using std::string;

using wechat::model::WeChatLoginUser;
using wechat::model::WeChatFriend;
using wechat::model::WeChatMessage;

using namespace wechat::parser::ios;

IOSMessageParser::IOSMessageParser(const WeChatLoginUser& u, WeChatFriend& f,
    BackupFileParser* p, IOSBackupArchives& archives) : MessageParser(u, f, p), iosArchives(archives)
{
}

void IOSMessageParser::parseSender(model::WeChatMessage& msg, bool isSender) const
{
    if (afriend.Type() == wechat::model::UserType::UserType_Group && !isSender)
    {
        string content = msg.getContent();
        size_t pos = content.find(":\n");
        if (pos != string::npos)
        {
            string senderName = content.substr(0, pos);
            msg.setSender(getSenderByName(senderName));
            msg.setContent(content.substr(pos + 2));
        }
    }
}

void IOSMessageParser::parseByText(model::WeChatMessage& msg) const
{

}

void IOSMessageParser::parseByImage(model::WeChatMessage& msg) const
{
    string baseFileName = "Documents/" + user.UserID() + "/Img/" + afriend.UserID() + "/" + msg.getExtra();
    string path;
    if (iosArchives.getAbsolutePathByRelativePath(baseFileName + ".pic", path))
    {
        msg.setMetadata("src", path);
    }
    if (iosArchives.getAbsolutePathByRelativePath(baseFileName + ".pic_thum", path))
    {
        msg.setMetadata("thumb", path);
    }
}

void IOSMessageParser::parseByAudio(model::WeChatMessage& msg) const
{
    string baseFileName = "Documents/" + user.UserID() + "/Audio/" + afriend.UserID() + "/" + msg.getExtra();
    string path;
    if (iosArchives.getAbsolutePathByRelativePath(baseFileName + ".aud", path))
    {
        msg.setMetadata("src", path);
    }

    int seconds = std::stoi(Utils::getXmlAttributeByPath(msg.getContent(), "/msg/voicemsg", "voicelength")) / 1000 + 0.5;
    msg.setMetadata("seconds", std::to_string(seconds));
}

void IOSMessageParser::parseByVideo(model::WeChatMessage& msg) const
{
    string senderName = Utils::getXmlAttributeByPath(msg.getContent(), "/msg/videomsg", "fromusername");
    if (!senderName.empty())
    {
        msg.setSender(getSenderByName(senderName));
    }

    string baseFileName = "Documents/" + user.UserID() + "/Video/" + afriend.UserID() + "/" + msg.getExtra();
    string path;
    if (iosArchives.getAbsolutePathByRelativePath(baseFileName + ".mp4", path))
    {
        msg.setMetadata("src", path);
    }
    if (iosArchives.getAbsolutePathByRelativePath(baseFileName + ".video_thum", path))
    {
        msg.setMetadata("thumb", path);
    }
}

void IOSMessageParser::parseByEmoticon(model::WeChatMessage& msg) const
{
    msg.setContent("[Emoticon]");
}

void IOSMessageParser::parseByAppMsg(model::WeChatMessage& msg) const
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
    msg.setMetadata("src", url);
    msg.setMetadata("thumb", thumburl);

    // [To-Do]
    msg.setContent("[App Message]");
}

void IOSMessageParser::parseBySystem(model::WeChatMessage& msg) const
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

    // [To-Do]
    msg.setContent("[System Message]");
}

void IOSMessageParser::parseByOther(model::WeChatMessage& msg) const
{
    // [To-Do]
    msg.setContent("[Other]");
}