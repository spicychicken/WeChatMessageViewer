#ifndef _CONTENT_WIDGET_H_
#define _CONTENT_WIDGET_H_

#include <QtWidgets/QWidget>

#include "wechat/model/Model.h"

class ContentWidget
{
public:
    static QWidget* createWidget(const wechat::model::WeChatMessage& message);
};

#endif