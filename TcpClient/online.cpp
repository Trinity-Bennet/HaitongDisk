#include "online.h"
#include "ui_online.h"
#include <qDebug>
#include "tcpclient.h"

online::online(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::online)
{
    ui->setupUi(this);
}

online::~online()
{
    delete ui;
}

void online::showUsr(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    uint uiSize = pdu->ui_msg_len / 32;
    ui->listWidget->clear();
    char name[32];
    for(uint i = 0;i < uiSize;i++){
        memcpy(name,(char*)(pdu->usr_data) + i * 32,32);
        ui->listWidget->addItem(name);
    }
}

void online::on_pushButton_clicked()
{
    QListWidgetItem *pItem = ui->listWidget->currentItem(); //获取好友列表当前行
    QString strFriendUsrName = pItem->text(); //想要添加用户的名称
    QString loginName = TcpClient::getInstance().getLoginName(); //当前登录用户的名称
    PDU * pdu = mkPDU(0);
    pdu->ui_msg_type = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->usr_data,strFriendUsrName.toStdString().c_str(),strFriendUsrName.size()); //请求加谁为好友
    memcpy(pdu->usr_data + 32,loginName.toStdString().c_str(),loginName.size()); //是谁请求加好友
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

