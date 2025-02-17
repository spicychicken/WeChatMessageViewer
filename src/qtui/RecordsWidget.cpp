#include "RecordsWidget.h"

#include <iostream>
#include <QDateTime>

#include "wechat/WeChatContext.h"
#include "wechat/model/Model.h"
#include "wechat/parser/BackupFileParser.h"
#include "ContentWidget.h"

static constexpr int UserRole_USER_ID = Qt::UserRole + 1;

RecordsWidget::RecordsWidget(QWidget* parent) : QWidget(parent)
{
    setupUi(this);

    itemModel = new QStandardItemModel();
    itemModel->setHorizontalHeaderLabels({"Head", "Name", "Content", "Time"});
    recordsTable->setModel(itemModel);

    recordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    recordsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);
    
    recordsTable->verticalHeader()->setDefaultSectionSize(60);
    recordsTable->setIconSize(QSize(60, 60));

    prevBtn->setEnabled(false);
    nextBtn->setEnabled(false);
}

RecordsWidget::~RecordsWidget()
{

}

void RecordsWidget::setCurrentUser(wechat::model::WeChatLoginUser* user)
{
    if (user != currentUser && user != nullptr)
    {
        currentUser = user;

        cleanWidget();
        updateListView();
    }
}

void RecordsWidget::cleanWidget()
{
    pageNums->clear();
    friendsList->clear();
    itemModel->removeRows(0, itemModel->rowCount());

    // friendImage->setPixmap(QPixmap(":icon/icons/DefaultProfileHead@2x.png"));
    friendName->setText("");
    friendTotal->setText("");
    friendStartDate->setText("");
    friendEndDate->setText("");
}

void RecordsWidget::updateListView()
{
    auto& friends = currentUser->getFriends();

    for (auto& f : friends)
    {
        if (f.Type() != wechat::model::UserType::UserType_Other && f.RecordCount() > 0)
        {
            QListWidgetItem* item = new QListWidgetItem();
            item->setText(QString::fromStdString(f.DisplayName()));
            item->setData(UserRole_USER_ID, QVariant(QString::fromStdString(f.UserID())));
            friendsList->addItem(item);
        }
    }
}

void RecordsWidget::updateRecordView()
{
    updateBasic();
    updatePages();
    updatePageButtons();
    updateTableView();
}

void RecordsWidget::updateBasic()
{
    friendImage->setPixmap(ContentWidget::fromWeChatUserHeadImg(currentFriend).scaled(friendImage->size(), Qt::KeepAspectRatio));

    friendName->setText(QString::fromStdString(currentFriend->DisplayName()));
    friendTotal->setText(QString::number(currentFriend->RecordCount()));
    friendStartDate->setText(QDateTime::fromSecsSinceEpoch(currentFriend->BeginTime()).toString("yyyy-MM-dd hh:mm:ss"));
    friendEndDate->setText(QDateTime::fromSecsSinceEpoch(currentFriend->LastTime()).toString("yyyy-MM-dd hh:mm:ss"));
}

void RecordsWidget::updatePages()
{
    pageNums->clear();

    page.index = 1;
    page.count = itemsPerPage->currentText().toInt();
    page.total = ceil((float)currentFriend->RecordCount() / page.count);

    for (int i = 0; i < page.total; ++i)
    {
        pageNums->addItem(QString::number(i + 1));
    }
}

void RecordsWidget::updatePageButtons()
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

void RecordsWidget::updateTableView()
{
    itemModel->removeRows(0, itemModel->rowCount());

    auto messages = sParser->loadFriendMessages(*currentUser, *currentFriend, page.index, page.count);
    for (int i = 0; i < messages.size(); ++i)
    {
        auto& message = messages[i];

        QList<QStandardItem*>   itemList;

        auto sender = message.getSender();
        auto itemIcon = new QStandardItem();
        itemIcon->setIcon(QIcon(ContentWidget::fromWeChatUserHeadImg(sender)));
        auto itemName = new QStandardItem(QString::fromStdString(sender->DisplayName()));
        itemList << itemIcon << itemName;

        auto itemContent = new QStandardItem("");
        itemList << itemContent;

        auto itemTime = new QStandardItem(QDateTime::fromSecsSinceEpoch(message.getTime()).toString("yyyy-MM-dd hh:mm:ss"));
        itemList << itemTime;

        itemModel->appendRow(itemList);

        recordsTable->setIndexWidget(itemModel->index(i, 2), ContentWidget::createWidget(currentFriend, message));
    }

    recordsTable->resizeRowsToContents();
}

void RecordsWidget::on_friendsList_itemClicked(QListWidgetItem* item)
{
    const std::string& userID = item->data(UserRole_USER_ID).toString().toStdString();
    currentFriend = &currentUser->getFriend(userID);

    if (currentFriend->Type() == wechat::model::UserType::UserType_Group)
    {
        // load group members
        if (!currentFriend->hasMemberData())
        {
            sParser->loadGroupMembers(*currentUser, *currentFriend);
        }
    }

    updateRecordView();
}

void RecordsWidget::on_prevBtn_clicked()
{
    if (page.index != 1)
    {
        page.index -= 1;

        updatePageButtons();
        updateTableView();

        pageNums->setCurrentText(QString::number(page.index));
    }
}

void RecordsWidget::on_nextBtn_clicked()
{
    if (page.index != page.total)
    {
        page.index += 1;

        updatePageButtons();
        updateTableView();

        pageNums->setCurrentText(QString::number(page.index));
    }
}

void RecordsWidget::on_pageNums_textActivated(const QString& text)
{
    if (text.toInt() != page.index)
    {
        page.index = text.toInt();

        updatePageButtons();
        updateTableView();
    }
}

void RecordsWidget::on_itemsPerPage_textActivated(const QString& text)
{
    if (currentFriend && text.toInt() != page.count)
    {
        page.index = 1;
        page.count = text.toInt();

        updatePages();
        updatePageButtons();
        updateTableView();
    }
}

/*
void RecordsWidget::resizeEvent(QResizeEvent *event)
{
    recordsTable->resizeRowsToContents();
    QWidget::resizeEvent(event);
}   */