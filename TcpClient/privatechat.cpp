#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>

PrivateChat::PrivateChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::setChatName(QString name)
{
    m_chatName = name;
    m_loginName = TcpClient::getInstance().getLoginName();
}

void PrivateChat::updateMsg(const PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    char loginName[32] = {'\0'};
    memcpy(loginName, pdu->usr_data + 32, 32);
    QString msg = QString("%1说: %2").arg(loginName).arg((char*)pdu->msg);
    ui->privateChatMsg->append(msg);
}

void PrivateChat::on_privateChatSendBtn_clicked()
{
    QString strMsg = ui->privateChatText->text();//获取发送信息
    if(!strMsg.isEmpty())
    {
        ui->privateChatText->clear();
        ui->privateChatMsg -> append(QString("我 : %2").arg(strMsg));
        PDU *pdu = mkPDU(strMsg.size() + 1);//+1是因为转为char后需要多一个\0结束符
        pdu->ui_msg_type = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        // 先复制好友的名称,再复制本人的名称,再写入聊天消息
        strncpy(pdu -> usr_data, m_chatName.toStdString().c_str(), 32); // 目标用户名
        strncpy(pdu -> usr_data + 32, m_loginName.toStdString().c_str(), 32); // 请求方用户名
        strncpy((char*)pdu->msg, strMsg.toStdString().c_str(), strMsg.size()); // 发送内容
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->ui_pdu_len);
        qDebug() << m_chatName << m_loginName;
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this, "私聊", "发送的内容不能为空");
    }

}

