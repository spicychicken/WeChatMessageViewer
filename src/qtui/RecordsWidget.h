#ifndef _RECORDSWIDGET_H_
#define _RECORDSWIDGET_H_

#include <QtWidgets/QWidget>
#include <QStandardItemModel>
#include "./ui_recordswidget.h"

namespace wechat::model
{
class WeChatLoginUser;
class WeChatFriend;
}

class WeChatViewerMainWindow;

class RecordsWidget : public QWidget, public Ui::RecordsWidget
{
    Q_OBJECT

public:
    RecordsWidget(WeChatViewerMainWindow* window, QWidget* parent = nullptr);
    virtual ~RecordsWidget();

    void setCurrentUser(wechat::model::WeChatLoginUser* user);

protected:
    // void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_friendsList_itemClicked(QListWidgetItem* item);
    void on_prevBtn_clicked();
    void on_nextBtn_clicked();
    void on_pageNums_activated(const QString& text);
    void on_itemsPerPage_activated(const QString& text);

private:
    void cleanWidget();
    void updateListView();          // friends list
    void updateRecordView();

    void updateBasic();
    void updatePages();
    void updatePageButtons();
    void updateTableView();

private:
    wechat::model::WeChatLoginUser*         currentUser = nullptr;
    wechat::model::WeChatFriend*            currentFriend = nullptr;
    WeChatViewerMainWindow*                 mainWindow = nullptr;
    QStandardItemModel*                     itemModel = nullptr;

    struct {
        int     count = 0;           // count per page
        int     total = 0;           // total page
        int     index = 0;           // current page
    } page;
};

#endif