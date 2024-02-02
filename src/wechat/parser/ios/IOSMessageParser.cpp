#include "IOSMessageParser.h"

#include <string>

using std::string;

using wechat::model::WeChatLoginUser;
using wechat::model::WeChatFriend;
using wechat::model::WeChatMessage;

using namespace wechat::parser::ios;

IOSMessageParser::IOSMessageParser(const WeChatLoginUser& u, WeChatFriend& f, BackupFileParser* p, IOSBackupArchives& archives)
                    : MessageParser(u, f, p), iosArchives(archives)
{
}

void IOSMessageParser::parseByImage(WeChatMessage& msg) const
{
    MessageParser::parseByImage(msg);

    string baseFileName = "Documents/" + user.UserID() + "/Img/" + afriend.UserID() + "/" + msg.getResourceID();
    string path;
    if (iosArchives.getAbsolutePathByRelativePath(baseFileName + ".pic", path))
    {
        msg.setSrc(path);
    }
    if (iosArchives.getAbsolutePathByRelativePath(baseFileName + ".pic_thum", path))
    {
        msg.setThumb(path);
    }
}

void IOSMessageParser::parseByAudio(WeChatMessage& msg) const
{
    MessageParser::parseByAudio(msg);

    string baseFileName = "Documents/" + user.UserID() + "/Audio/" + afriend.UserID() + "/" + msg.getResourceID();
    string path;
    if (iosArchives.getAbsolutePathByRelativePath(baseFileName + ".aud", path))
    {
        msg.setSrc(path);
    }
}

void IOSMessageParser::parseByVideo(WeChatMessage& msg) const
{
    MessageParser::parseByVideo(msg);

    string baseFileName = "Documents/" + user.UserID() + "/Video/" + afriend.UserID() + "/" + msg.getResourceID();
    string path;
    if (iosArchives.getAbsolutePathByRelativePath(baseFileName + ".mp4", path))
    {
        msg.setSrc(path);
    }
    if (iosArchives.getAbsolutePathByRelativePath(baseFileName + ".video_thum", path))
    {
        msg.setThumb(path);
    }
}
