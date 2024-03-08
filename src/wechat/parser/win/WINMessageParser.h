#ifndef WINMESSAGEPARSER_H_
#define WINMESSAGEPARSER_H_

#include "../MessageParser.h"

namespace wechat::parser::win
{

class WINMessageParser : public MessageParser
{
public:
    explicit WINMessageParser(const model::WeChatLoginUser& u, model::WeChatFriend& f, BackupFileParser* p, const std::string& path);

private:
    void parseSender(model::WeChatMessage& msg) const override;
    void parseByImage(model::WeChatMessage& msg) const override;
    void parseByAudio(model::WeChatMessage& msg) const override;
    void parseByVideo(model::WeChatMessage& msg) const override;

private:
    std::string         basePath;
};

}

#endif