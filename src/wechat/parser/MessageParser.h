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

    model::WeChatMessage parse(int createTime, const std::string& message, int des, int type, const std::string& resID) const;

protected:
    void parseBasic(model::WeChatMessage& msg, int createTime, const std::string& message, int des, int type, const std::string& resID) const;
    void parseByType(model::WeChatMessage& msg) const;
    
    virtual void parseByText(model::WeChatMessage& msg) const;
    virtual void parseByImage(model::WeChatMessage& msg) const;
    virtual void parseByAudio(model::WeChatMessage& msg) const;
    virtual void parseByVideo(model::WeChatMessage& msg) const;
    virtual void parseByEmoticon(model::WeChatMessage& msg) const;
    virtual void parseByAppMsg(model::WeChatMessage& msg) const;
    virtual void parseBySystem(model::WeChatMessage& msg) const;

protected:
    model::WeChatFriend&                afriend;
    const model::WeChatLoginUser&       user;
    BackupFileParser*                   backupParser;
};

}

#endif