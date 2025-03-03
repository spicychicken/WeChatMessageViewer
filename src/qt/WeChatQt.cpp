#include "WeChatQt.h"

#include <QDateTime>

#include "wechat/WeChatContext.h"
#include "functions/Utils.h"

WeChatQt::WeChatQt()
{
}

WeChatQt::~WeChatQt()
{
}

int WeChatQt::detectBackupType(const QUrl& url)
{
    return static_cast<int>(sWECHAT.detectBackupType(url.toLocalFile().toStdString()));
}

bool WeChatQt::initContextFromPath(const QUrl& url)
{
    return sWECHAT.initContextFromPath(url.toLocalFile().toStdString());
}

QStringList WeChatQt::listLoginUserNames()
{
    std::vector<std::string> loginUsers = sWECHAT.listLoginUserNames();
    QStringList     stringList;
    for (const auto& user : loginUsers)
    {
        stringList.append(QString::fromStdString(user));
    }
    return stringList;
}

QString WeChatQt::detectLoginUserSecretKey()
{
    // only for windows, and convert to hex string
    return QString::fromStdString(Utils::stringToHexString(sBackup->getMetadata("rawKey")));
}

QVariantMap weChatUserToQVariantMap(const wechat::model::WeChatUser* user)
{
    QVariantMap     variantMap;
    variantMap["userID"] = QString::fromStdString(user->UserID());
    variantMap["userName"] = QString::fromStdString(user->UserName());
    variantMap["displayName"] = QString::fromStdString(user->DisplayName());
    variantMap["headImg"] = QString::fromStdString(user->HeadImg());
    variantMap["msgCount"] = user->RecordCount();
    variantMap["beginTime"] = QDateTime::fromSecsSinceEpoch(user->BeginTime()).toString("yyyy-MM-dd hh:mm:ss");
    variantMap["lastTime"] = QDateTime::fromSecsSinceEpoch(user->LastTime()).toString("yyyy-MM-dd hh:mm:ss");
    return variantMap;
}

QVariantMap metadatasToVariantMap(const std::unordered_map<std::string, std::string>& metadatas)
{
    QVariantMap     variantMap;
    for (auto& m : metadatas)
    {
        variantMap[QString::fromStdString(m.first)] = QString::fromStdString(m.second);
    }
    return variantMap;
} 

QVariantMap WeChatQt::loadLoginUser(const QString& loginUserName, const QString& secretKey)
{
    wechat::model::WeChatLoginUser* loginUser = sWECHAT.loadLoginUser(loginUserName.toStdString(), 
                                                            Utils::hexStringToString(secretKey.toStdString()));
    
    // convert WeChatLoginUser to QVariantMap
    QVariantMap  loginUserMap = weChatUserToQVariantMap(loginUser);
    return loginUserMap;
}

QVariantMap WeChatQt::listFriends(int start, int count, bool filterZero)
{
    const std::vector<wechat::model::WeChatFriend>& friends = sWECHAT.listFriends();

    QVariantMap  results;

    results["total"] = friends.size();

    QVariantList    friendsList;
    for (int i = 0; i < count && (start + i) < friends.size(); i++)
    {
        const auto& f = friends[start + i];

        QVariantMap     friendMap = weChatUserToQVariantMap(&f);

        friendsList.append(friendMap);
    }
    results["msg"] = friendsList;

    return results;
}

QVariantMap WeChatQt::listMessages(const QString& friendID, int page, int count)
{
    const auto& aFriend = sWECHAT.getFriendByID(friendID.toStdString());
    auto messages = sWECHAT.listMessages(friendID.toStdString(), page, count);

    QVariantMap  results;

    results["total"] = aFriend.RecordCount();

    QVariantList    messagesList;
    for (auto& m : messages)
    {
        QVariantMap     messageMap;

        messageMap["type"] = static_cast<int>(m.getType());
        messageMap["sender"] = weChatUserToQVariantMap(m.getSender());
        messageMap["content"] = QString::fromStdString(m.getContent());
        messageMap["msgSvrID"] = QString::fromStdString(m.getMsgSvrID());
        messageMap["time"] = QDateTime::fromSecsSinceEpoch(m.getTime()).toString("yyyy-MM-dd hh:mm:ss");
        messageMap["metadatas"] = metadatasToVariantMap(m.getMetadatas());

        messagesList.append(messageMap);
    }
    results["msg"] = messagesList;

    return results;
}

void WeChatQt::playAudio(const QString& friendID, const QVariantMap& message)
{
    wechat::model::WeChatMessage weChatMessage;
    if (message.contains("msgSvrID")) {
        weChatMessage.setMsgSvrID(message["msgSvrID"].toString().toStdString());
    }
    if (message.contains("src")) {
        weChatMessage.setMetadata("src", message["src"].toString().toStdString());
    }
    if (message.contains("dbPath")) {
        weChatMessage.setMetadata("dbPath", message["dbPath"].toString().toStdString());
    }

    sWECHAT.playAudio(friendID.toStdString(), weChatMessage);
}

WeChatQt* WeChatQt::instance() {
    static WeChatQt sWeChatQt;
    return &sWeChatQt;
}