#ifndef _FRIENDSWIDGET_H_
#define _FRIENDSWIDGET_H_

#include <QtWidgets/QWidget>
#include <QStandardItemModel>
#include "./ui_friendswidget.h"

namespace wechat::model
{
class WeChatLoginUser;
}

class FriendsWidget : public QWidget, public Ui::FriendsWidget
{
    Q_OBJECT

public:
    FriendsWidget(QWidget* parent = nullptr);
    virtual ~FriendsWidget();

    void setCurrentUser(wechat::model::WeChatLoginUser* user);

    // void updateModel(int countPerPage, int currentPage);

private slots:
    void on_prevBtn_clicked();
    void on_nextBtn_clicked();

    void on_pageNums_textActivated(const QString& text);
    void on_itemsPerPage_textActivated(const QString& text);

private:
    void updateWidget();

    void updatePages();
    void updatePageButtons();
    void updateTableView();

    // void resetPageNumsContent();

private:
    wechat::model::WeChatLoginUser*     currentUser = nullptr;
    QStandardItemModel*                 itemModel = nullptr;

    struct {
        int     count = 0;           // count per page
        int     total = 0;           // total page
        int     index = 0;           // current page
    } page;
};

#endif