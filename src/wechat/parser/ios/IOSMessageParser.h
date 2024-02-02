#ifndef IOSMESSAGEPARSER_H_
#define IOSMESSAGEPARSER_H_

#include "../MessageParser.h"
#include "IOSBackupArchives.h"

namespace wechat
{
namespace parser
{
namespace ios
{
class IOSMessageParser : public MessageParser
{
public:
    explicit IOSMessageParser(const model::WeChatLoginUser& u, model::WeChatFriend& f, BackupFileParser* p, IOSBackupArchives& archives);

private:
    void parseByImage(model::WeChatMessage& msg) const override;
    void parseByAudio(model::WeChatMessage& msg) const override;
    void parseByVideo(model::WeChatMessage& msg) const override;

private:
    IOSBackupArchives& iosArchives;
};

}
}
}

#endif