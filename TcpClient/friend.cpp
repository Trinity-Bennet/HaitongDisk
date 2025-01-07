#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QDebug>
#include <QString>
#include "privatechat.h"
#include <QMessageBox>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pFriendListWidget = new QListWidget;
    m_pInputMsgLE = new QLineEdit;
    m_pShowMsgTE = new QTextEdit;
    m_pDelFriendBtn = new QPushButton("删除好友");
    m_pFlushFriendBtn = new QPushButton("刷新在线好友");
    m_pShowOnlineUserBtn = new QPushButton("显示在线用户");
    m_pSearchUserBtn = new QPushButton("查找用户");
    m_pMsgSendBtn = new QPushButton("信息发送");
    m_pPrivateChatBtn = new QPushButton("私聊");
    m_pOnline = new online;

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pShowOnlineUserBtn);
    pRightPBVBL->addWidget(m_pFlushFriendBtn);
    pRightPBVBL->addWidget(m_pSearchUserBtn);
    pRightPBVBL->addWidget(m_pPrivateChatBtn);
    pRightPBVBL->addWidget(m_pDelFriendBtn);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendBtn);

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    m_pOnline->hide();
    pMain->addWidget(m_pOnline);

    setLayout(pMain);//显示布局
    connect(m_pShowOnlineUserBtn,SIGNAL(clicked(bool)),this,SLOT(showOnline()));//显示在线用户按钮的关联事件
    connect(m_pSearchUserBtn,SIGNAL(clicked(bool)),this,SLOT(searchUsr()));//搜索用户按钮的关联事件
    connect(m_pFlushFriendBtn,SIGNAL(clicked(bool)),this,SLOT(refreshFriendList()));//搜索用户按钮的关联事件
    connect(m_pDelFriendBtn,SIGNAL(clicked(bool)),this,SLOT(deleteFriend()));//删除用户按钮关联的事件
    connect(m_pPrivateChatBtn,SIGNAL(clicked(bool)),this,SLOT(privateChat()));//私聊按钮关联的事件
    connect(m_pMsgSendBtn,SIGNAL(clicked(bool)),this,SLOT(groupChat()));//发送消息按钮关联的事件

}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    m_pOnline->showUsr(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    m_pFriendListWidget->clear();
    uint uiSize = pdu->ui_msg_len / 32;
    char name[32] = {'\0'};
    for(uint i = 0;i < uiSize;i++){
        memcpy(name,(char *)(pdu->usr_data) + i * 32,32);
        m_pFriendListWidget->addItem(name);
    }
}

void Friend::updateGroupMsg(PDU *pdu)
{
    QString msg = QString("%1 说： %2").arg(pdu->usr_data).arg((char*)pdu->msg);
    m_pShowMsgTE->append(msg);
}

QListWidget *Friend::getFriendList()
{
    return m_pFriendListWidget;
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden()){
        m_pOnline->show();
        PDU * pdu = mkPDU(0);
        pdu->ui_msg_type = ENUM_MSG_TYPE_ONLINE_USR_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
        free(pdu);
        pdu = NULL;
    }
    else{
        m_pOnline->hide();
    }
}

void Friend::searchUsr()
{
    m_strSearchName = QInputDialog::getText(this,"搜索","用户名称"); //获取用户输入，返回得到的文本
    if(!m_strSearchName.isEmpty()){
        qDebug() << m_strSearchName;
        PDU *pdu = mkPDU(0);
        memcpy(pdu->usr_data,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        pdu->ui_msg_type = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
        free(pdu);
        pdu = NULL;
    }
}

void Friend::refreshFriendList()
{
    // 首先获取当前用户的用户名
    QString loginName = TcpClient::getInstance().getLoginName();
    // qDebug() << loginName;
    PDU * pdu = mkPDU(0);
    pdu->ui_msg_type = ENUM_MSG_TYPE_REFRESH_FRIEND_REQUEST;
    memcpy(pdu->usr_data,loginName.toStdString().c_str(),loginName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);//向服务器发送刷新好友列表的请求
    // qDebug() << "刷新请求已发送！";
    free(pdu);
    pdu = NULL;
}

void Friend::deleteFriend()
{
    if(m_pFriendListWidget->currentItem() == NULL){
        return;
    }
    QString friendName = m_pFriendListWidget->currentItem()->text(); //获取好友名称
    QString loginName = TcpClient::getInstance().getLoginName(); //获取当前用户名称
    qDebug() << friendName << " " << loginName;
    PDU* pdu = mkPDU(0);
    pdu -> ui_msg_type = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    strncpy(pdu -> usr_data, friendName.toStdString().c_str(), 32);
    strncpy(pdu -> usr_data + 32, loginName.toStdString().c_str(), 32);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

void Friend::privateChat()
{
    if(m_pFriendListWidget->currentItem() == NULL){ // 没有选择私聊对象
        QMessageBox::warning(this,"私聊","请选择私聊对象！");
        return;
    }
    else{
        QString strChatName = m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().setChatName(strChatName);
        if(PrivateChat::getInstance().isHidden()){//如果私聊界面隐藏，那就将其显示
            PrivateChat::getInstance().show();
        }
    }
}

void Friend::groupChat()
{
    QString msg = m_pInputMsgLE->text();
    if(msg.isEmpty()){
        QMessageBox::warning(this,"群聊","信息不能为空！");
        return;
    }
    QString myMsg = QString("我 说： %2").arg(msg);
    m_pShowMsgTE->append(myMsg);
    PDU * pdu = mkPDU(msg.size() + 1);
    pdu->ui_msg_type = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
    QString sourceName = TcpClient::getInstance().getLoginName();
    strncpy(pdu->usr_data,sourceName.toStdString().c_str(),32);//谁发的消息
    strncpy((char *)pdu->msg,msg.toStdString().c_str(),msg.size());
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
    m_pInputMsgLE->clear();
}
