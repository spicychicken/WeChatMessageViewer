#ifndef WECHAT_CONTEXT_H_
#define WECHAT_CONTEXT_H_

#include <vector>

#include "wechat/model/Model.h"

namespace wechat
{

namespace parser
{
class BackupFileParser;
}

class WeChatContext
{
public:
    static WeChatContext& get();

    model::BackupType detectBackupType(const std::string& path);
    bool initContextFromPath(const std::string& path);
    std::vector<std::string> listLoginUserNames();
    model::WeChatLoginUser* loadLoginUser(const std::string& loginUserName, const std::string& secretKey);
    const model::WeChatFriend& getFriendByID(const std::string& friendID);
    const std::vector<model::WeChatFriend>& listFriends();
    std::vector<model::WeChatMessage> listMessages(const std::string& friendID, int start, int count);

    void playAudio(const std::string& friendID, const model::WeChatMessage& message);
    std::string loadMsgImgData(const std::string& fileName);

    ////////////////////////////////////////////
    void createParserFromPath(const std::string& path);

    void loadBackup();
    void loadLoginUsers();
    void loadCurrentLoginUserFriends();

    void switchCurrentLoginUser(model::WeChatLoginUser* newLoginUser);

public:
    const std::unordered_map<std::string, model::WeChatLoginUser> getLoginUsers() const;

    model::WeChatLoginUser* getCurrentUser()       { return currentUser; }
    model::WeChatBackup* getBackup()               { return &backup; }
    parser::BackupFileParser* getParser()          { return backupParser; }

private:
    // wechat - variable
    parser::BackupFileParser*               backupParser = nullptr;
    model::WeChatLoginUser*                 currentUser = nullptr;
    model::WeChatBackup                     backup;
};

#define sWECHAT wechat::WeChatContext::get()
#define sCurrentUser wechat::WeChatContext::get().getCurrentUser()
#define sParser wechat::WeChatContext::get().getParser()
#define sBackup wechat::WeChatContext::get().getBackup()

}
#endif