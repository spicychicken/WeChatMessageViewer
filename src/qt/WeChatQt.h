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
    Q_INVOKABLE QStringList listLoginUserNames();

    Q_INVOKABLE QString detectLoginUserSecretKey();

    Q_INVOKABLE QVariantMap loadLoginUser(const QString& loginUserName, const QString& secretKey);
    Q_INVOKABLE QVariantMap listFriends(int start, int count, bool filterZero);
    Q_INVOKABLE QVariantMap listMessages(const QString& friendID, int page, int count);

    Q_INVOKABLE bool playAudio(const QString& friendID, const QVariantMap& message);

    Q_INVOKABLE bool fileExist(const QString& fileName);

    static WeChatQt* instance();
};

#endif