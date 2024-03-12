#ifndef WECHAT_CONTEXT_H_
#define WECHAT_CONTEXT_H_

#include <string>

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

    void createParserFromPath(const std::string& path);

    void loadBackup();
    void loadLoginUsers();
    void loadCurrentLoginUserFriends();

    bool switchCurrentLoginUser(const std::string& newUserID);

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