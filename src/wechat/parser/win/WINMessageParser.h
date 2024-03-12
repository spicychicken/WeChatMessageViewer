#ifndef WINMESSAGEPARSER_H_
#define WINMESSAGEPARSER_H_

#include "../MessageParser.h"
#include "WINBackupArchives.h"

namespace wechat::parser::win
{

class WINMessageParser : public MessageParser
{
public:
    explicit WINMessageParser(const model::WeChatLoginUser& u, model::WeChatFriend& f,
            BackupFileParser* p, WINBackupArchives& archives);

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
    WINBackupArchives&              winArchives;
    std::vector<std::string>        usedUserNames;
};

}

#endif