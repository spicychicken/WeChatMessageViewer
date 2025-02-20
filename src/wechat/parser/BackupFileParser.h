#ifndef WECHATBACKUPPARSER_H
#define WECHATBACKUPPARSER_H

#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

#include "wechat/model/Model.h"

namespace wechat::parser
{

constexpr static char LOCAL_DEFAULT_HEAD_IMAGE[] = ":icon/icons/DefaultProfileHead@2x.png";

using ParserStateNotifer = std::function<void (const std::string message)>;

class BackupFileParser
{
public:
    virtual bool loadBackup(model::WeChatBackup& backup) = 0;
    virtual std::vector<std::string> listLoginUsers(model::WeChatBackup& backup) = 0;
    virtual model::WeChatLoginUser& loadLoginUser(model::WeChatBackup& backup, const std::string& loginUserName, const std::string& secretKey = "") = 0;

    virtual void loadLoginUsers(model::WeChatBackup& backup) = 0;
    virtual void loadUserFriends(model::WeChatLoginUser& user) = 0;

    virtual void loadGroupMembers(const model::WeChatLoginUser& user, model::WeChatFriend& afriend) = 0;
    virtual void loadGroupMember(const model::WeChatLoginUser& user, model::WeChatFriend& afriend, const std::string& memberName) = 0;
    virtual std::vector<model::WeChatMessage> loadFriendMessages(const model::WeChatLoginUser& user, model::WeChatFriend& afriend, int page, int countPerPage = 20) = 0;

    virtual std::string loadUserHeadImgData(const model::WeChatLoginUser& user, const model::WeChatUser& userOrFriend) = 0;
    virtual std::string loadUserAudioData(const model::WeChatLoginUser& user, const model::WeChatFriend& afriend, const model::WeChatMessage& message) = 0;

    void setNotifer(ParserStateNotifer n) {
        notifer = n;
    }

    void notifyStateChange(const std::string& message) {
        if (notifer) {
            notifer(message);
        }
    }

protected:
    void updateLoginUserRecord(model::WeChatLoginUser& user);

private:
    ParserStateNotifer      notifer;
};

model::BackupType detectBackupType(const std::string& path);
BackupFileParser* createParser(const std::string& path);

}

#endif // WECHATBACKUPPARSER_H
