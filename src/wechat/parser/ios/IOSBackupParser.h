#ifndef IOSBACKUPPARSER_H
#define IOSBACKUPPARSER_H

#include <string>
#include <unordered_map>

#include "../BackupFileParser.h"

namespace wechat::parser::ios
{

class IOSBackupParser : public BackupFileParser
{
public:
    explicit IOSBackupParser(const std::string& path);

    void loadBackup(model::WeChatBackup& backup) override;
    void loadLoginUsers(model::WeChatBackup& backup) override;
    void loadUserFriends(model::WeChatLoginUser& user) override;

    void loadGroupMembers(const model::WeChatLoginUser& user, model::WeChatFriend& afriend) override;
    void loadGroupMember(const model::WeChatLoginUser& user, model::WeChatFriend& afriend, const std::string& memberName) override;
    std::vector<model::WeChatMessage> loadFriendMessages(const model::WeChatLoginUser& user, model::WeChatFriend& afriend, int page, int countPerPage = 20) override;

    std::string loadUserHeadImgData(const model::WeChatLoginUser* user, const model::WeChatUser* userOrFriend) override;
    std::string loadUserAudioData(const model::WeChatLoginUser* user, const model::WeChatFriend* afriend, const model::WeChatMessage& message) override;

private:
    void loadLoginUsersFromMMDB(std::unordered_map<std::string, model::WeChatLoginUser>& users);
    void loadLoginUsersFromLoginInfo2(std::unordered_map<std::string, model::WeChatLoginUser>& users);
    void loadLoginUserDetailsFromMMsetting(std::unordered_map<std::string, model::WeChatLoginUser>& users);

    void loadUserFriendsFromSessionDB(const std::string& userID, std::unordered_map<std::string, model::WeChatFriend>& friends);
    void loadUserFriendsFromMessageDB(const std::string& userID, std::unordered_map<std::string, model::WeChatFriend>& friends);
    void loadUserFriendsFromContactDB(const std::string& userID, std::unordered_map<std::string, model::WeChatFriend>& friends);

private:
    std::string                         backupPath;
};

}

#endif // IOSBackupParser
