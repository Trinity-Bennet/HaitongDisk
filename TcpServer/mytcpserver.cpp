#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer() {

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

// 当有新的连接可用，会调用该槽函数
void MyTcpServer::incomingConnection(qintptr socketDiscriptor){// 服务器产生一个传输数据的套接字，返回文件描述符
    qDebug() << "new client connected";
    MyTcpSocket * pTcpSocket = new MyTcpSocket; //建立新的socket连接
    pTcpSocket->setSocketDescriptor(socketDiscriptor); //设置socket描述符
    m_tcpSocketList.append(pTcpSocket); //将当前socket对象加入List
    connect(pTcpSocket,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteSocket(MyTcpSocket*)));//用户一旦下线就删除套接字
}

void MyTcpServer::addUsrMsgSend(const char *name, PDU *pdu)
{
    if(name == NULL || pdu == NULL){
        return;
    }
    QString strName = name;
    for(int i = 0;i<m_tcpSocketList.size();i++){
        if(strName == m_tcpSocketList.at(i)->getName()){
            m_tcpSocketList.at(i)->write((char*)pdu,pdu->ui_pdu_len);
            // qDebug() << "希望加为好友!" << pdu -> usr_data << " " << pdu -> usr_data + 32;
            break;
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket *m_socket)
{
    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for(;iter != m_tcpSocketList.end();iter++){
        if(m_socket == *iter){
            (*iter)->deleteLater();//删除指针指向的对象
            *iter = NULL;
            m_tcpSocketList.erase(iter);//删除列表里的套接字
            break;
        }
    }
    for(int i = 0;i<m_tcpSocketList.size();i++){
        qDebug() << m_tcpSocketList.at(i)->getName();
    }
}
