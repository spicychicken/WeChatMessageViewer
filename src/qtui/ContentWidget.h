#ifndef _CONTENT_WIDGET_H_
#define _CONTENT_WIDGET_H_

#include <QtWidgets/QWidget>

#include "wechat/model/Model.h"

class QPixmap;

class ContentWidget
{
public:
    static QWidget* createWidget(const wechat::model::WeChatFriend* afriend, const wechat::model::WeChatMessage& message);

    static QPixmap fromWeChatUserHeadImg(const wechat::model::WeChatUser* userOrFriend);
};

#endif