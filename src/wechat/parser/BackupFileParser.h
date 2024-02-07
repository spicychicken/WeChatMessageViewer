#ifndef WECHATBACKUPPARSER_H
#define WECHATBACKUPPARSER_H

#include <string>
#include <functional>
#include <unordered_map>

#include "wechat/model/Model.h"

namespace wechat::parser
{

enum class ParserType
{
    ParserType_IOS,
};

using ParserStateNotifer = std::function<void (const std::string message)>;

class BackupFileParser
{
public:
    virtual void loadBackup(model::WeChatBackup& backup) = 0;
    virtual void loadLoginUsers(model::WeChatBackup& backup) = 0;
    virtual void loadUserFriends(model::WeChatLoginUser& user) = 0;

    virtual void loadGroupMembers(const model::WeChatLoginUser& user, model::WeChatFriend& afriend) = 0;
    virtual void loadGroupMember(const model::WeChatLoginUser& user, model::WeChatFriend& afriend, const std::string& memberName) = 0;
    virtual std::vector<model::WeChatMessage> loadFriendMessages(const model::WeChatLoginUser& user, model::WeChatFriend& afriend, int page, int countPerPage = 20) = 0;

    void setNotifer(ParserStateNotifer n) {
        notifer = n;
    }

    void notifyStateChange(const std::string& message) {
        if (notifer) {
            notifer(message);
        }
    }

private:
    ParserStateNotifer      notifer;
};

BackupFileParser* createParser(ParserType type, const std::string& path);

}

#endif // WECHATBACKUPPARSER_H
