#include "WeChatQt.h"

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

QVariantMap WeChatQt::loadLoginUser(const QString& loginUserName, const QString& secretKey)
{
    wechat::model::WeChatLoginUser* loginUser = sWECHAT.loadLoginUser(loginUserName.toStdString(), secretKey.toStdString());
    
    // convert WeChatLoginUser to QVariantMap
    QVariantMap  loginUserMap;
    return loginUserMap;
}

QStringList WeChatQt::listFriends()
{
    std::vector<std::string> friends = sWECHAT.listFriends();
    QStringList     stringList;
    for (const auto& user : friends)
    {
        stringList.append(QString::fromStdString(user));
    }
    return stringList;
}

WeChatQt* WeChatQt::instance() {
    static WeChatQt sWeChatQt;
    return &sWeChatQt;
}