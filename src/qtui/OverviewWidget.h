#ifndef _OVERVIEWWIDGET_H_
#define _OVERVIEWWIDGET_H_

#include <QtWidgets/QWidget>
#include "./ui_overviewwidget.h"

namespace wechat::model
{
class WeChatLoginUser;
}

class OverviewWidget : public QWidget, public Ui::OverviewWidget
{
    Q_OBJECT

public:
    OverviewWidget(QWidget* parent = nullptr);
    virtual ~OverviewWidget();

    void setCurrentUser(wechat::model::WeChatLoginUser* user);

    void clearDisplay();
    void updateDisplay();

private:
    wechat::model::WeChatLoginUser*     currentUser = nullptr;
};

#endif