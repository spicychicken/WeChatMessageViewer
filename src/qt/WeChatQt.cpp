#include "WeChatQt.h"

#include <QDateTime>

#include "wechat/WeChatContext.h"

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

QStringList WeChatQt::listLoginUsers()
{
    std::vector<std::string> loginUsers = sWECHAT.listLoginUsers();
    QStringList     stringList;
    for (const auto& user : loginUsers)
    {
        stringList.append(QString::fromStdString(user));
    }
    return stringList;
}

QString WeChatQt::detectLoginUserSecretKey()
{
    // only for windows
    return QString::fromStdString(sBackup->getMetadata("rawKey"));
}

void weChatUserToQVariantMap(const wechat::model::WeChatUser* user, QVariantMap& variantMap)
{
    variantMap["userID"] = QString::fromStdString(user->UserID());
    variantMap["userName"] = QString::fromStdString(user->UserName());
    variantMap["displayName"] = QString::fromStdString(user->DisplayName());

    if (!user->HeadImgUrlHD().empty()) {
        variantMap["headImg"] = QString::fromStdString(user->HeadImgUrlHD());
    }
    else {
        variantMap["headImg"] = QString::fromStdString(user->HeadImgUrl());
    }
}

QVariantMap WeChatQt::loadLoginUser(const QString& loginUserName, const QString& secretKey)
{
    wechat::model::WeChatLoginUser* loginUser = sWECHAT.loadLoginUser(loginUserName.toStdString(), secretKey.toStdString());
    
    // convert WeChatLoginUser to QVariantMap
    QVariantMap  loginUserMap;
    weChatUserToQVariantMap(loginUser, loginUserMap);
    return loginUserMap;
}

QVariantMap WeChatQt::listFriends(int start, int count, bool filterZero)
{
    const std::vector<wechat::model::WeChatFriend>& friends = sWECHAT.listFriends();

    QVariantMap  results;

    results["total"] = friends.size();

    QVariantList    friendsList;
    for (int i = 0; i < count; i++)
    {
        const auto& f = friends[start + i];

        QVariantMap     friendMap;

        weChatUserToQVariantMap(&f, friendMap);
        friendMap["msgCount"] = f.RecordCount();

        if (f.RecordCount() != 0)
        {
            friendMap["beginTime"] = QDateTime::fromSecsSinceEpoch(f.BeginTime()).toString("yyyy-MM-dd hh:mm:ss");
            friendMap["lastTime"] = QDateTime::fromSecsSinceEpoch(f.LastTime()).toString("yyyy-MM-dd hh:mm:ss");
        }
        else
        {
            friendMap["beginTime"] = "";
            friendMap["lastTime"] = "";
        }

        friendsList.append(friendMap);
    }
    results["msg"] = friendsList;

    return results;
}

WeChatQt* WeChatQt::instance() {
    static WeChatQt sWeChatQt;
    return &sWeChatQt;
}