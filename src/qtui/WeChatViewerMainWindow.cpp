#include "WeChatViewerMainWindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <iostream>
#include <future>
#include <functional>

#include "OverviewWidget.h"
#include "RecordsWidget.h"
#include "FriendsWidget.h"
#include "ContentWidget.h"

#include "wechat/WeChatContext.h"

#include "wechat/parser/BackupFileParser.h"
#include "constants.h"
#include "functions/Utils.h"

#include "audio/OpenAL.h"

WeChatViewerMainWindow::WeChatViewerMainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    qRegisterMetaType<AsyncActions>("AsyncActions");

    connect(this, SIGNAL(status_message(const QString&)), this, SLOT(do_status_message(const QString&)), Qt::ConnectionType::QueuedConnection);
    connect(this, SIGNAL(async_actions_done(AsyncActions)), this, SLOT(do_async_actions_done(AsyncActions)), Qt::ConnectionType::QueuedConnection);
}

WeChatViewerMainWindow::~WeChatViewerMainWindow()
{
}

void WeChatViewerMainWindow::on_selectBKFileBtn_clicked()
{
    QString path = QFileDialog::getExistingDirectory(nullptr, tr("select IOS backup folder"), ".");
    if (path.length() != 0)
    {
        sWECHAT.createParserFromPath(path.toStdString());

        if (sParser)
        {
            sWECHAT.loadBackup();

            executeAsyncActions(AsyncActions::AsyncActions_LoadUser, []() { sWECHAT.loadLoginUsers(); });
        }
        else
        {
            QMessageBox::critical(this, "Unknown Folder Type", "please select another path!!!");
        }
    }
}

void WeChatViewerMainWindow::on_userNameListCB_activated(const QString& text)
{
    std::string currentDisplayName = text.toStdString();
    if (displayToUserIDs.count(currentDisplayName))
    {
        if (sWECHAT.switchCurrentLoginUser(displayToUserIDs[currentDisplayName]))
        {
            iTuneVersion->setText(QString::fromStdString(sBackup->getITuneVersion()));
            productVersion->setText(QString::fromStdString(sBackup->getProductVersion()));
            lastBackupDate->setText(QString::fromStdString(sBackup->getLastBackupDate()));

            userHead->setPixmap(ContentWidget::fromWeChatUserHeadImg(sCurrentUser).scaled(userHead->size(), Qt::KeepAspectRatio));

            if (tabWidget->currentIndex() == 0)
            {
                on_tabWidget_currentChanged(0);
            }
            else
            {
                tabWidget->setCurrentIndex(0);
            }
        }
    }
}

void WeChatViewerMainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 0)
    {
        showOverviewWidget();
    }
    else if (index == 1)
    {
        showRecordsWidget();
    }
    else if (index == 2)
    {
        showFriendsWidget();
    }
}

void WeChatViewerMainWindow::showOverviewWidget() {
    if (overviewWidget == nullptr) {
        overviewWidget = new OverviewWidget();
    }

    if (sCurrentUser && !sCurrentUser->hasFriendData())
    {
        executeAsyncActions(AsyncActions::AsyncActions_LoadFriend, []() { sWECHAT.loadCurrentLoginUserFriends(); });
    }
    else
    {
        overviewWidget->setCurrentUser(sCurrentUser);
    }

    switchToSubWidget(overviewWidget);
}

void WeChatViewerMainWindow::showRecordsWidget() {
    if (recordsWidget == nullptr) {
        recordsWidget = new RecordsWidget();
    }

    recordsWidget->setCurrentUser(sCurrentUser);

    switchToSubWidget(recordsWidget);
}

void WeChatViewerMainWindow::showFriendsWidget() {
    if (friendsWidget == nullptr) {
        friendsWidget = new FriendsWidget();
    }

    friendsWidget->setCurrentUser(sCurrentUser);

    switchToSubWidget(friendsWidget);
}

void WeChatViewerMainWindow::switchToSubWidget(QWidget* newWidget) {
    if (currentWidget != nullptr && currentWidget == newWidget)
    {
        return;
    }

    auto currentTab = tabWidget->widget(tabWidget->currentIndex());
    if (sCurrentUser && currentTab->layout()->count() == 0)
    {
        currentTab->layout()->addWidget(newWidget);
    }

    currentWidget = newWidget;
}

void WeChatViewerMainWindow::do_status_message(const QString& message) {
    statusBar()->showMessage(message, 10);
}

void WeChatViewerMainWindow::do_async_actions_done(AsyncActions action)
{
    if (action == AsyncActions::AsyncActions_LoadUser)
    {
        auto& users = sWECHAT.getLoginUsers();
        for (auto& user : users)
        {
            userNameListCB->addItem(QString::fromStdString(user.second.DisplayName()));
            displayToUserIDs[user.second.DisplayName()] = user.first;
        }
        on_userNameListCB_activated(userNameListCB->currentText());
    }
    else if (action == AsyncActions::AsyncActions_LoadFriend)
    {
        overviewWidget->setCurrentUser(sCurrentUser);
    }
    else if (action == AsyncActions::AsyncActions_LoadMessage)
    {

    }
}

void WeChatViewerMainWindow::executeAsyncActions(AsyncActions action, std::function<void(void)> func)
{
    // If the std::future obtained from std::async is not moved from or bound to a reference, 
    // the destructor of the std::future will block at the end of the full expression 
    // until the asynchronous operation completes,
    // essentially making code such as the following synchronous:
    std::thread([](auto func, auto action, auto window) {
        func();
        emit window->async_actions_done(action);
    }, func, action, this).detach();
}