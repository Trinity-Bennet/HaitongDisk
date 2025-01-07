#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include "protocol.h"

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChat(QWidget *parent = nullptr);
    ~PrivateChat();
    static PrivateChat &getInstance();
    void setChatName(QString name);//设置聊天对象的名称
    void updateMsg(const PDU * pdu);//显示对方私聊消息

private slots:
    void on_privateChatSendBtn_clicked();

private:
    Ui::PrivateChat *ui;
    QString m_chatName;//聊天对象的名称
    QString m_loginName; //当前发送消息对象的名称
};

#endif // PRIVATECHAT_H
