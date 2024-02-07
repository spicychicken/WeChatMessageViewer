#ifndef _DETAILS_H_
#define _DETAILS_H_

#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>

namespace wechat::parser::ios::details
{

struct SessionRecord
{
    std::string     conStrRes1;
    long            createTime;
    int             unReadCount;
    std::string     userName;
};

struct FriendRecord
{
    std::string     userName;
    std::string     dbContactChatRoom;
    std::string     dbContactLocal;
    std::string     dbContactRemark;
    std::string     dbContactProfile;
    std::string     dbContactSocial;
    std::string     dbContactHeadImage;
    int             type;
};

struct ChatRecord
{
    int             createTime;
    std::string     message;
    int             des;
    int             type;
    std::string     mesLocalID;
};

std::vector<SessionRecord> getSessionRecordsFromDB(const std::string& dbPath);
std::string getNSObjectsByKey(const std::unordered_map<std::string, std::string>& setting, const std::string& value);
std::vector<std::string> getSessionIDsFromDB(const std::string& dbPath);
bool getFriendRecordByName(const std::string& dbPath, const std::string& friendName, FriendRecord& record);
std::vector<FriendRecord> getFriendRecordsByNames(const std::string& dbPath, const std::vector<std::string>& friendNames);
std::unordered_map<std::string, FriendRecord> getFriendRecordsFromDB(const std::string& dbPath);
std::vector<ChatRecord> getChatRecordsByFriend(const std::string& dbPath, const std::string& friendID, int page, int countPerPage = 20);

std::vector<int> getChatRecordInfoByFriend(const std::string& dbPath, const std::string& friendID);

}

#endif