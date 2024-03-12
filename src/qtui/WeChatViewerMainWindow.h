#ifndef WECHATVIEWERMAINWINDOW_H
#define WECHATVIEWERMAINWINDOW_H

#include <string>
#include <functional>
#include <unordered_map>

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>

#include "./ui_wechatviewermainwindow.h"

namespace wechat::model
{
    class WeChatBackup;
    class WeChatLoginUser;
}

namespace wechat::parser
{
    class BackupFileParser;
}

class OverviewWidget;
class RecordsWidget;
class FriendsWidget;

enum class AsyncActions
{
    AsyncActions_LoadUser,
    AsyncActions_LoadFriend,
    AsyncActions_LoadMessage,
};

class WeChatViewerMainWindow : public QMainWindow, public Ui::WeChatViewerMainWindow
{
    Q_OBJECT

public:
    WeChatViewerMainWindow(QWidget *parent = nullptr);
    ~WeChatViewerMainWindow();

signals:
    void status_message(const QString& message);
    void async_actions_done(AsyncActions actions);

private slots:
    void on_selectBKFileBtn_clicked();
    void on_userNameListCB_activated(const QString& text);
    void on_tabWidget_currentChanged(int index);

    void do_status_message(const QString& message);
    void do_async_actions_done(AsyncActions action);

public:
    // async actions, execute in another thread
    void executeAsyncActions(AsyncActions action, std::function<void(void)> func);

private:
    void showOverviewWidget();
    void showRecordsWidget();
    void showFriendsWidget();

private:
    std::unordered_map<std::string, std::string>    displayToUserIDs;

    // UI - variable
private:
    QWidget*                        currentWidget = nullptr;
    
    OverviewWidget*                 overviewWidget = nullptr;
    RecordsWidget*                  recordsWidget = nullptr;
    FriendsWidget*                  friendsWidget = nullptr;   

    void switchToSubWidget(QWidget* newWidget);
};
#endif // WECHATVIEWERMAINWINDOW_H
