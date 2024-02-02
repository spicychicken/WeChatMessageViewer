#ifndef _OVERVIEWWIDGET_H_
#define _OVERVIEWWIDGET_H_

#include <QtWidgets/QWidget>
#include "./ui_overviewwidget.h"

namespace wechat
{
namespace model
{
class WeChatLoginUser;
}
}

class WeChatViewerMainWindow;

class OverviewWidget : public QWidget, public Ui::OverviewWidget
{
    Q_OBJECT

public:
    OverviewWidget(WeChatViewerMainWindow* window, QWidget* parent = nullptr);
    virtual ~OverviewWidget();

    void setCurrentUser(wechat::model::WeChatLoginUser* user);

    void clearDisplay();
    void updateDisplay();

private:
    wechat::model::WeChatLoginUser*     currentUser = nullptr;
    WeChatViewerMainWindow*             mainWindow = nullptr;
};

#endif