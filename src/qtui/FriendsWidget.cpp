#include "FriendsWidget.h"

#include <QDateTime>

#include "wechat/WeChatContext.h"
#include "wechat/model/Model.h"
#include "ContentWidget.h"

#include <iostream>

FriendsWidget::FriendsWidget(QWidget* parent) : QWidget(parent)
{
    setupUi(this);

    itemModel = new QStandardItemModel();
    itemModel->setHorizontalHeaderLabels({"UserName", "Head", "NickName", "Location", "Count", "From", "To"});

    friendsTable->setModel(itemModel);
    friendsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    friendsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::ResizeToContents);

    friendsTable->verticalHeader()->setDefaultSectionSize(60);
    friendsTable->setIconSize(QSize(60, 60));
}

FriendsWidget::~FriendsWidget()
{
    delete itemModel;
}

void FriendsWidget::setCurrentUser(wechat::model::WeChatLoginUser* user)
{
    if (user != currentUser && user != nullptr)
    {
        currentUser = user;

        updateWidget();
    }
}

void FriendsWidget::updateWidget()
{
    updatePages();
    updatePageButtons();
    updateTableView();
}

void FriendsWidget::updatePages()
{
    pageNums->clear();

    page.index = 1;
    page.count = itemsPerPage->currentText().toInt();
    page.total = ceil((float)currentUser->getFriends().size() / page.count);

    for (int i = 0; i < page.total; ++i)
    {
        pageNums->addItem(QString::number(i + 1));
    }
}

void FriendsWidget::updatePageButtons()
{
    if (page.index == 1 && page.total == 1)
    {
        prevBtn->setEnabled(false);
        nextBtn->setEnabled(false);
    }
    else if (page.index == 1)
    {
        prevBtn->setEnabled(false);
        nextBtn->setEnabled(true);
    }
    else if (page.index == page.total)
    {
        prevBtn->setEnabled(true);
        nextBtn->setEnabled(false);
    }
    else
    {
        prevBtn->setEnabled(true);
        nextBtn->setEnabled(true);
    }
}

std::string getLocation(const wechat::model::WeChatFriend& afriend)
{
    std::string location;
    if (!afriend.Country().empty())
    {
        location += afriend.Country();
    }

    if (!afriend.Province().empty())
    {
        if (location.empty())
        {
            location += afriend.Province();
        }
        else
        {
            location += ("/" + afriend.Province());
        }
    }

    if (!afriend.City().empty())
    {
        if (location.empty())
        {
            location += afriend.City();
        }
        else
        {
            location += ("/" + afriend.City());
        }
    }

    return location;
}

void FriendsWidget::updateTableView()
{
    itemModel->removeRows(0, itemModel->rowCount());

    auto& friends = currentUser->getFriends();
    for (int i = 0; i < page.count; ++i)
    {
        int pos = (page.index - 1) * page.count + i;
        if (pos >= friends.size())
        {
            break;
        }

        auto& afriend = friends[pos];

        QList<QStandardItem*>   itemList;
        auto itemID = new QStandardItem(QString::fromStdString(afriend.UserName()));
        itemList << itemID;

        auto itemIcon = new QStandardItem();
        itemIcon->setIcon(QIcon(ContentWidget::fromWeChatUserHeadImg(&afriend)));
        itemList << itemIcon;

        auto itemName = new QStandardItem(QString::fromStdString(afriend.DisplayName()));
        itemList << itemName;

        auto itemLocation = new QStandardItem(QString::fromStdString(getLocation(afriend)));
        itemList << itemLocation;

        auto itemCount = new QStandardItem(QString::number(afriend.RecordCount()));
        itemList << itemCount;

        auto itemBegin = new QStandardItem(QDateTime::fromSecsSinceEpoch(afriend.BeginTime()).toString("yyyy-MM-dd hh:mm:ss"));
        itemList << itemBegin;

        auto itemEnd = new QStandardItem(QDateTime::fromSecsSinceEpoch(afriend.LastTime()).toString("yyyy-MM-dd hh:mm:ss"));
        itemList << itemEnd;

        itemModel->appendRow(itemList);
    }
}

void FriendsWidget::on_prevBtn_clicked()
{
    if (page.index != 1)
    {
        page.index -= 1;

        updatePageButtons();
        updateTableView();

        pageNums->setCurrentText(QString::number(page.index));
    }
}

void FriendsWidget::on_nextBtn_clicked()
{
    if (page.index != page.total)
    {
        page.index += 1;

        updatePageButtons();
        updateTableView();

        pageNums->setCurrentText(QString::number(page.index));
    }
}

void FriendsWidget::on_pageNums_textActivated(const QString& text)
{
    if (text.toInt() != page.index)
    {
        page.index = text.toInt();

        updatePageButtons();
        updateTableView();
    }
}

void FriendsWidget::on_itemsPerPage_textActivated(const QString& text)
{
    if (text.toInt() != page.count)
    {
        page.index = 1;
        page.count = text.toInt();

        updateWidget();
    }
}