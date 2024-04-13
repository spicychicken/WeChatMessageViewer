#include "OverviewWidget.h"

#include <iostream>

#include <QDateTime>

#include "wechat/model/Model.h"
#include "ContentWidget.h"

OverviewWidget::OverviewWidget(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
}

OverviewWidget::~OverviewWidget()
{
}

void OverviewWidget::setCurrentUser(wechat::model::WeChatLoginUser* user)
{
    if (user != currentUser && user != nullptr)
    {
        currentUser = user;

        clearDisplay();
        updateDisplay();
    }
}

static void updateListWidget(QListWidget* listWidget, const std::vector<wechat::model::WeChatFriend>& friends)
{
    listWidget->clear();

    for (auto& afriend : friends)
    {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString::fromStdString(afriend.DisplayName()));
        item->setIcon(QIcon(ContentWidget::fromWeChatUserHeadImg(&afriend)));

        listWidget->addItem(item);
    }
}

void OverviewWidget::clearDisplay()
{
    topFriendContact->clear();
    topGroupContact->clear();
    recentFriendContact->clear();
    recentGroupContact->clear();
}

void OverviewWidget::updateDisplay()
{
    userID->setText(QString::fromStdString(currentUser->UserID()));
    userNickName->setText(QString::fromStdString(currentUser->NickName()));
    userAliasName->setText(QString::fromStdString(currentUser->AliasName()));
    userRecordCount->setText(QString::number(currentUser->RecordCount()));
    userFriendCount->setText(QString::number(currentUser->getFriendsCountByType(wechat::model::UserType::UserType_Friend)));
    userGroupCount->setText(QString::number(currentUser->getFriendsCountByType(wechat::model::UserType::UserType_Group)));
    userRecordStartDate->setText(QDateTime::fromSecsSinceEpoch(currentUser->BeginTime()).toString("yyyy-MM-dd hh:mm:ss"));
    userRecordEndDate->setText(QDateTime::fromSecsSinceEpoch(currentUser->LastTime()).toString("yyyy-MM-dd hh:mm:ss"));

    // top 10 chat to friend
    auto friends = currentUser->getTopFriends(wechat::model::UserType::UserType_Friend, 10, [](auto& a, auto&b) {
        return a.RecordCount() > b.RecordCount();
    });
    updateListWidget(topFriendContact, friends);

    // top 10 chat to group
    friends = currentUser->getTopFriends(wechat::model::UserType::UserType_Group, 10, [](auto& a, auto&b) {
        return a.RecordCount() > b.RecordCount();
    });
    updateListWidget(topGroupContact, friends);

    // recent 10 chat to friend
    friends = currentUser->getTopFriends(wechat::model::UserType::UserType_Friend, 10, [](auto& a, auto&b) {
        return a.LastTime() > b.LastTime();
    });
    updateListWidget(recentFriendContact, friends);

    // recent 10 chat to group
    friends = currentUser->getTopFriends(wechat::model::UserType::UserType_Group, 10, [](auto& a, auto&b) {
        return a.LastTime() > b.LastTime();
    });
    updateListWidget(recentGroupContact, friends);

    // area(Country/Province/City), gender, age
}