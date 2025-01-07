#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include<QTextEdit>
#include<QListWidget>
#include<QLineEdit>
#include<QPushButton>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include "online.h"


class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUsr(PDU * pdu);
    void updateFriendList(PDU * pdu);//更新好友列表
    void updateGroupMsg(PDU * pdu);//显示群聊信息

    QString m_strSearchName;//搜索用户的名称
    QListWidget * getFriendList();

signals:
public slots:
    void showOnline(); //显示在线用户
    void searchUsr(); //搜索用户
    void refreshFriendList(); //刷新好友列表
    void deleteFriend(); //删除好友
    void privateChat(); //私聊
    void groupChat(); //群聊
private:
    QTextEdit *m_pShowMsgTE;//显示信息
    QListWidget *m_pFriendListWidget;//好友列表
    QLineEdit *m_pInputMsgLE;//内容输入框
    QPushButton *m_pDelFriendBtn;//删除用户按钮
    QPushButton *m_pFlushFriendBtn;//刷新好友用户按钮
    QPushButton *m_pShowOnlineUserBtn;//查看在线用户
    QPushButton *m_pSearchUserBtn;//搜素用户
    QPushButton *m_pMsgSendBtn;//信息发送
    QPushButton *m_pPrivateChatBtn;//私聊
    online *m_pOnline;//在线用户
};

#endif // FRIEND_H
