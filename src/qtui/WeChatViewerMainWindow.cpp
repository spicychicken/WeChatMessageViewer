#include "WeChatViewerMainWindow.h"

#include <QFileDialog>
#include <iostream>
#include <future>
#include <functional>

#include <fmt/core.h>

#include "OverviewWidget.h"
#include "RecordsWidget.h"
#include "FriendsWidget.h"

#include "wechat/parser/BackupFileParser.h"
#include "constants.h"
#include "functions/Utils.h"

WeChatViewerMainWindow::WeChatViewerMainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    qRegisterMetaType<AsyncActions>("AsyncActions");

    connect(this, SIGNAL(status_message(const QString&)), this, SLOT(do_status_message(const QString&)), Qt::ConnectionType::QueuedConnection);
    connect(this, SIGNAL(async_actions_done(AsyncActions)), this, SLOT(do_async_actions_done(AsyncActions)), Qt::ConnectionType::QueuedConnection);

    backup = new wechat::model::WeChatBackup();
}

WeChatViewerMainWindow::~WeChatViewerMainWindow()
{
    delete backup;
}

void WeChatViewerMainWindow::on_selectBKFileBtn_clicked()
{
    QString path = QFileDialog::getExistingDirectory(nullptr, tr("select IOS backup folder"), ".");
    if (path.length() != 0)
    {
        backupParser = wechat::parser::createParser(wechat::parser::ParserType::ParserType_IOS, path.toStdString());
        backupParser->setNotifer(std::bind([](const std::string& message, auto mainWindow) {
            emit mainWindow->status_message(QString::fromStdString(message));
        }, std::placeholders::_1, this));
        backupParser->loadBackup(*backup);

        executeAsyncActions(AsyncActions::AsyncActions_LoadUser, std::bind([](auto backupParser, auto& backup) {
            backupParser->loadLoginUsers(*backup);
        }, backupParser, std::ref(backup)));
    }
}

void WeChatViewerMainWindow::on_userNameListCB_activated(const QString& text)
{
    std::string currentUserName = text.toStdString();
    if (((currentUser != nullptr && currentUser->UserName() != currentUserName) || (currentUser == nullptr)) && nameToIDs.count(currentUserName))
    {
        currentUser = &backup->getLoginUserByID(nameToIDs[currentUserName]);

        userHead->setPixmap(QPixmap(QString::fromStdString(currentUser->LocalHeadImg()))
                    .scaled(userHead->size(), Qt::KeepAspectRatio));

        iTuneVersion->setText(QString::fromStdString(backup->getITuneVersion()));
        productVersion->setText(QString::fromStdString(backup->getProductVersion()));
        lastBackupDate->setText(QString::fromStdString(backup->getLastBackupDate()));

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
        overviewWidget = new OverviewWidget(this);
    }

    if (currentUser && !currentUser->hasFriendData())
    {
        executeAsyncActions(AsyncActions::AsyncActions_LoadFriend, std::bind([](auto& user, auto backupParser) {
            backupParser->loadUserFriends(*user);
        }, std::ref(currentUser), backupParser));
    }
    else
    {
        overviewWidget->setCurrentUser(currentUser);
    }

    switchToSubWidget(overviewWidget);
}

void WeChatViewerMainWindow::showRecordsWidget() {
    if (recordsWidget == nullptr) {
        recordsWidget = new RecordsWidget(this);
    }

    recordsWidget->setCurrentUser(currentUser);

    switchToSubWidget(recordsWidget);
}

void WeChatViewerMainWindow::showFriendsWidget() {
    if (friendsWidget == nullptr) {
        friendsWidget = new FriendsWidget(this);
    }

    friendsWidget->setCurrentUser(currentUser);

    switchToSubWidget(friendsWidget);
}

void WeChatViewerMainWindow::switchToSubWidget(QWidget* newWidget) {
    if (currentWidget != nullptr && currentWidget == newWidget)
    {
        return;
    }

    auto currentTab = tabWidget->widget(tabWidget->currentIndex());
    if (currentUser && currentTab->layout()->count() == 0)
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
        auto& users = backup->getLoginUsers();
        for (auto& user : users)
        {
            userNameListCB->addItem(QString::fromStdString(user.second.DisplayName()));
            nameToIDs[user.second.DisplayName()] = user.first;
        }
        on_userNameListCB_activated(userNameListCB->currentText());
    }
    else if (action == AsyncActions::AsyncActions_LoadFriend)
    {
        overviewWidget->setCurrentUser(currentUser);
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