#include "WINMessageParser.h"

#include <string>
#include <regex>

using std::string;

using wechat::model::WeChatLoginUser;
using wechat::model::WeChatFriend;
using wechat::model::WeChatMessage;
using wechat::model::ChatMessageType;

using namespace wechat::parser::win;

WINMessageParser::WINMessageParser(const WeChatLoginUser& u, WeChatFriend& f, BackupFileParser* p, const string& path)
                    : MessageParser(u, f, p), basePath(path)
{
}

void WINMessageParser::parseSender(model::WeChatMessage& msg) const
{
    MessageParser::parseSender(msg);

    if (msg.getType() == ChatMessageType::Text && afriend.Type() == wechat::model::UserType::UserType_Group)
    {
        string extra = msg.getExtra();
        string senderName = extra.substr(12, extra[11]);

        if (!senderName.empty())
        {
            msg.setSender(getSenderByName(senderName));
        }
    }
}

void WINMessageParser::parseByImage(WeChatMessage& msg) const
{
    MessageParser::parseByImage(msg);
}

void WINMessageParser::parseByAudio(WeChatMessage& msg) const
{
    MessageParser::parseByAudio(msg);

}

void WINMessageParser::parseByVideo(WeChatMessage& msg) const
{
    MessageParser::parseByVideo(msg);

    string extra = msg.getExtra();
    
    std::smatch  results;
    if (std::regex_search(extra, results, std::regex(user.UserName() + ".*?(\.jpg)")))
    {
        string thumb = results[0].str();
        string rest = extra.substr(extra.find(thumb) + thumb.length());
        if (std::regex_search(rest, results, std::regex(user.UserName() + ".*?(\.mp4)"))) {
            msg.setSrc(basePath + "/" + results[0].str());
        }
        msg.setThumb(basePath + "/" + thumb);
    }
    else
    {
        if (std::regex_search(extra, results, std::regex(user.UserName() + ".*?(\.mp4)"))) {
            msg.setSrc(basePath + "/" + results[0].str());
        }
    }
}