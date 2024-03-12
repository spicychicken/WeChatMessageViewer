#include "ContentWidget.h"

#include <QLabel>
#include <iostream>

#include "wechat/parser/MessageParser.h"
#include "wechat/WeChatContext.h"
#include "audio/OpenAL.h"

using wechat::parser::MessageParser;
using wechat::model::ChatMessageType;

static std::string BROSWER_PATH = "D:/tools/Mozilla Firefox/firefox.exe";
static std::string PARAMS_SEPARATOR = "<|>";

void widgetLinkActivatedForOther(const std::string& fileLink)
{
    audio::OpenAL::stop();

    if (!fileLink.empty())
    {
        std::string command = "\"\"" + BROSWER_PATH + "\" \"file:///" + fileLink + "\"\"";
        system(command.c_str());
    }
}

static void widgetLinkActivatedForAudio(const wechat::model::WeChatFriend* afriend, const wechat::model::WeChatMessage& message)
{
    audio::OpenAL::stop();

    if (sBackup->getBackupType() == wechat::model::BackupType::BackupType_WIN)
    {
        auto data = sParser->loadUserAudioData(sCurrentUser, afriend, message);
        audio::OpenAL::singlePlaySilkFromData(data);
    }
    else
    {
        if (!message.getSrc().empty())
        {
            audio::OpenAL::singlePlaySilkFromPath(message.getSrc());
        }
    }
}

static QWidget* createTextWidget(const wechat::model::WeChatMessage& message)
{
    QLabel* qlabel = new QLabel(QString::fromStdString(message.getContent()));
    qlabel->setStyleSheet("QLabel{padding:4px;border-radius:4px}");
    qlabel->setWordWrap(true);
    return qlabel;
}

static QWidget* createImageWidget(const wechat::model::WeChatMessage& message)
{
    auto src = message.getSrc().empty() ? message.getThumb() : message.getSrc();
    QLabel* qlabel = new QLabel("<a href='" + QString::fromStdString(src) + "'>[Image]</a>");
    qlabel->setToolTip("<img src='" + QString::fromStdString(message.getThumb()) + "' />");
    qlabel->setStyleSheet("QLabel{padding:4px;border-radius:4px}");
    qlabel->connect(qlabel, &QLabel::linkActivated, [](const QString& link) {
        widgetLinkActivatedForOther(link.toStdString());
    });
    return qlabel;
}

static QWidget* createVideoWidget(const wechat::model::WeChatMessage& message)
{
    QLabel* qlabel = new QLabel();
    if (message.getSrc().empty())
    {
        qlabel->setText("<a href=''>[Video] <span>(Video Missing)</span></a>");
    }
    else
    {
        qlabel->setText("<a href='" + QString::fromStdString(message.getSrc()) + "'>[Video]</a>");
    }
    qlabel->setToolTip("<img src='" + QString::fromStdString(message.getThumb()) + "' />");
    qlabel->setStyleSheet("QLabel{padding:4px;border-radius:4px}");
    qlabel->connect(qlabel, &QLabel::linkActivated, [](const QString& link) {
        widgetLinkActivatedForOther(link.toStdString());
    });
    return qlabel;
}

static QWidget* createAudioWidget(const wechat::model::WeChatFriend* afriend, const wechat::model::WeChatMessage& message)
{
    /* std::string audioSource = message.getSrc();
    if (sBackup->getBackupType() == wechat::model::BackupType::BackupType_WIN)
    {
        audioSource = message.getDbPath() + PARAMS_SEPARATOR + message.getMsgSvrID();
    }   */

    QLabel* qlabel = new QLabel(QString::fromStdString("<a href='audio'>[Audio]</a>"));
    qlabel->setStyleSheet("QLabel{padding:4px;border-radius:4px}");
    qlabel->connect(qlabel, &QLabel::linkActivated, [=](const QString& link) {
        widgetLinkActivatedForAudio(afriend, message);
    });
    return qlabel;
}

static QWidget* createAppMsgWidget(const wechat::model::WeChatMessage& message)
{
    std::string labelTxt = "<a href=''>[" + message.getContent() + "]</a>";
    QLabel* qlabel = new QLabel(QString::fromStdString(labelTxt));
    qlabel->setStyleSheet("QLabel{padding:4px;border-radius:4px}");
    return qlabel;
}

static QWidget* createSystemWidget(const wechat::model::WeChatMessage& message)
{
    QLabel* qlabel = new QLabel("<a href='" + QString::fromStdString(message.getSrc()) + "'>[Audio]</a>");
    qlabel->setStyleSheet("QLabel{padding:4px;border-radius:4px}");
    return qlabel;
}

QWidget* ContentWidget::createWidget(const wechat::model::WeChatFriend* afriend, const wechat::model::WeChatMessage& message)
{
    switch (message.getType())
    {
    case ChatMessageType::Text:
        return createTextWidget(message);
        break;
    case ChatMessageType::Image:
        return createImageWidget(message);
        break;
    case ChatMessageType::Video:
        return createVideoWidget(message);
        break;
    case ChatMessageType::Audio:
        return createAudioWidget(afriend, message);
        break;
    case ChatMessageType::AppMessage:
        return createAppMsgWidget(message);
        break;
    case ChatMessageType::System:
    case ChatMessageType::System_Recalled:
        return createTextWidget(message);
        break;
    default:
        break;
    }
    return createTextWidget(message);
}

QPixmap ContentWidget::fromWeChatUserHeadImg(const wechat::model::WeChatUser* userOrFriend)
{
    std::string localHeadImg = userOrFriend->LocalHeadImg();
    if (localHeadImg.empty())
    {
        std::string imgData = sParser->loadUserHeadImgData(sCurrentUser, userOrFriend);
        return QPixmap::fromImage(QImage::fromData((unsigned char*)imgData.c_str(), imgData.length()));
    }
    else {
        return QPixmap(QString::fromStdString(localHeadImg));
    }
}
