#ifndef WINBACKUPPARSER_H
#define WINBACKUPPARSER_H

#include <string>
#include <unordered_map>

#include "../BackupFileParser.h"

namespace wechat::parser::win
{

class WINBackupParser : public BackupFileParser
{
public:
    explicit WINBackupParser(const std::string& path);

    bool loadBackup(model::WeChatBackup& backup) override;
    std::vector<std::string> listLoginUserNames(model::WeChatBackup& backup) override;
    model::WeChatLoginUser& loadLoginUser(model::WeChatBackup& backup, const std::string& loginUserName, const std::string& secretKey = "") override;

    void loadLoginUsers(model::WeChatBackup& backup) override;
    void loadUserFriends(model::WeChatLoginUser& user) override;

    void loadGroupMembers(const model::WeChatLoginUser& user, model::WeChatFriend& afriend) override;
    void loadGroupMember(const model::WeChatLoginUser& user, model::WeChatFriend& afriend, const std::string& memberName) override;
    std::vector<model::WeChatMessage> loadFriendMessages(const model::WeChatLoginUser& user, model::WeChatFriend& afriend, int page, int countPerPage = 20) override;

    std::string loadUserHeadImgData(const model::WeChatLoginUser& user, const model::WeChatUser& userOrFriend) override;
    std::string loadUserAudioData(const model::WeChatLoginUser& user, const model::WeChatFriend& afriend, const model::WeChatMessage& message) override;
    std::string loadMsgImgData(const std::string& fileName) override;

private:
    void loadUserFriendsFromSession(const model::WeChatLoginUser& user, std::unordered_map<std::string, model::WeChatFriend>& friends);
    void loadUserFriendsDetailsFromMsg(const model::WeChatLoginUser& user, std::unordered_map<std::string, model::WeChatFriend>& friends);

    bool loadUserFriendFromContact(const model::WeChatLoginUser& user, model::WeChatUser& afriend);

private:
    std::string                                     backupPath;
    std::unordered_map<std::string, std::string>    rawKeys;
    std::string                                     defaultPass;
};

}

#endif // WINBACKUPPARSER_H
