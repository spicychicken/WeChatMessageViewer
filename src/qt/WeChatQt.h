#ifndef _WECHAT_QT_H_
#define _WECHAT_QT_H_

#include <QObject>
#include <QVariantMap>
#include <QUrl>

class WeChatQt: public QObject
{
    Q_OBJECT

    WeChatQt();
    ~WeChatQt();

public:
    Q_INVOKABLE int detectBackupType(const QUrl& url);
    Q_INVOKABLE bool initContextFromPath(const QUrl& url);
    Q_INVOKABLE QStringList listLoginUsers();

    Q_INVOKABLE QString detectLoginUserSecretKey();
    Q_INVOKABLE QVariantMap loadLoginUser(const QString& loginUserName, const QString& secretKey);
    Q_INVOKABLE QVariantMap listFriends(int start, int count, bool filterZero);

    static WeChatQt* instance();
};

#endif