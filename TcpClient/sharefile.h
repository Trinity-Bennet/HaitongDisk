#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QListWidget>

class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);
    static ShareFile & getInstance();
    void updateFriend(QListWidget * pFriendList); //更新好友列表

signals:
public slots:
    void cancelSelectFriend();//取消选择
    void selectAll(); //全选
    void commitList(); //提交好友名
    void cancelList(); //取消提交
private:
    QPushButton * mSelectAllBtn; //全选
    QPushButton * mCancelSelectionBtn; //取消选择
    QPushButton * mCommitBtn; //确认
    QPushButton * mCancelBtn; //取消

    QScrollArea * mFriendArea; //好友区
    QWidget * mFriendW; //好友窗口
    QButtonGroup * mManageFriend; //管理好友界面
    QVBoxLayout * mFriendWVBL;//好友信息垂直布局
};

#endif // SHAREFILE_H
