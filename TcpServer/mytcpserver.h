#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();
    static MyTcpServer &getInstance();

    void incomingConnection(qintptr socketDiscriptor);

    void addUsrMsgSend(const char * name,PDU * pdu); //添加在线用户时转发添加请求
public slots:
    void deleteSocket(MyTcpSocket * m_socket);

private:
    QList<MyTcpSocket*> m_tcpSocketList;

};

#endif // MYTCPSERVER_H
