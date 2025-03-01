#ifndef _WECHAT_MESSAGE_PARSER_H_
#define _WECHAT_MESSAGE_PARSER_H_

#include <string>
#include <functional>

#include "wechat/model/Model.h"
#include "BackupFileParser.h"

namespace wechat::parser
{

class MessageParser
{
public:
    MessageParser(const model::WeChatLoginUser& u, model::WeChatFriend& f, BackupFileParser* p);

    model::WeChatMessage parse(const std::string& msgSvrID, int createTime, const std::string& content, bool isSender, int type, const std::string& extra) const;

protected:
    void parseBasic(model::WeChatMessage& msg, const std::string& msgSvrID, int createTime, const std::string& content, bool isSender, int type, const std::string& extra) const;
    void parseByType(model::WeChatMessage& msg) const;

    virtual void parseSender(model::WeChatMessage& msg, bool isSender) const = 0;
    virtual void parseByText(model::WeChatMessage& msg) const = 0;
    virtual void parseByImage(model::WeChatMessage& msg) const = 0;
    virtual void parseByAudio(model::WeChatMessage& msg) const = 0;
    virtual void parseByVideo(model::WeChatMessage& msg) const = 0;
    virtual void parseByEmoticon(model::WeChatMessage& msg) const = 0;
    virtual void parseByAppMsg(model::WeChatMessage& msg) const = 0;
    virtual void parseBySystem(model::WeChatMessage& msg) const = 0;

    virtual void parseByOther(model::WeChatMessage& msg) const = 0;

protected:
    const model::WeChatUser* getSenderByID(const std::string& senderID) const;
    const model::WeChatUser* getSenderByName(const std::string& senderName) const;

protected:
    model::WeChatFriend&                afriend;
    const model::WeChatLoginUser&       user;
    BackupFileParser*                   backupParser;
};

}

#endif