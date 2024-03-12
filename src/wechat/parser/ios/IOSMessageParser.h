#ifndef IOSMESSAGEPARSER_H_
#define IOSMESSAGEPARSER_H_

#include "../MessageParser.h"
#include "IOSBackupArchives.h"

namespace wechat::parser::ios
{

class IOSMessageParser : public MessageParser
{
public:
    explicit IOSMessageParser(const model::WeChatLoginUser& u, model::WeChatFriend& f, 
            BackupFileParser* p, IOSBackupArchives& archives);

private:
    void parseSender(model::WeChatMessage& msg, bool isSender) const override;
    void parseByText(model::WeChatMessage& msg) const override;
    void parseByImage(model::WeChatMessage& msg) const override;
    void parseByAudio(model::WeChatMessage& msg) const override;
    void parseByVideo(model::WeChatMessage& msg) const override;
    void parseByEmoticon(model::WeChatMessage& msg) const override;
    void parseByAppMsg(model::WeChatMessage& msg) const override;
    void parseBySystem(model::WeChatMessage& msg) const override;
    void parseByOther(model::WeChatMessage& msg) const override;

private:
    IOSBackupArchives& iosArchives;
};

}

#endif