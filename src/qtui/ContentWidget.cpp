#include "ContentWidget.h"

#include <QLabel>
#include <wechat/parser/MessageParser.h>

#include <iostream>

#include "audio/OpenAL.h"

using wechat::parser::MessageParser;
using wechat::model::ChatMessageType;

static std::string BROSWER_PATH = "D:/tools/Mozilla Firefox/firefox.exe";
void widgetLinkActivated(int type, const QString& link)
{
    std::string fileLink = link.toStdString();
    if (!fileLink.empty())
    {
        audio::OpenAL::stop();
        if (type == 1)
        {
            // audio
            audio::OpenAL::singlePlaySilk(fileLink);
        }
        else
        {
            // [To-Do]
            system(("\"" + BROSWER_PATH + "\" file:///" + fileLink).c_str());
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
        widgetLinkActivated(0, link);
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
        widgetLinkActivated(0, link);
    });
    return qlabel;
}

static QWidget* createAudioWidget(const wechat::model::WeChatMessage& message)
{
    QLabel* qlabel = new QLabel("<a href='" + QString::fromStdString(message.getSrc()) + "'>[Audio]</a>");
    qlabel->setStyleSheet("QLabel{padding:4px;border-radius:4px}");
    qlabel->connect(qlabel, &QLabel::linkActivated, [](const QString& link) {
        widgetLinkActivated(1, link);
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

QWidget* ContentWidget::createWidget(const wechat::model::WeChatMessage& message)
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
        return createAudioWidget(message);
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
    return nullptr;
}