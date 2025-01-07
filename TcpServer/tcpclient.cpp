#include "tcpclient.h"
#include "ui_tcpclient.h"
#include "protocol.h"

#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();

    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));

    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_port); // 连接服务器
}

TcpClient::~TcpClient()
{
    delete ui;
}

// 加载套接字信息
void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray data = file.readAll();
        QString strData = data.toStdString().c_str(); // 右边转换成指向C风格字符串的指针
        // qDebug() << strData;
        QStringList strList = strData.split("\r\n"); // 在\r\n处切割字符串
        m_strIP = strList.at(0);
        m_port = strList.at(1).toUShort();
        // qDebug() << m_strIP;
        // qDebug() << m_port;
        file.close();
    }
    else{
        QMessageBox::critical(this,"open config","open config failed!");
    }
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功！");
}

void TcpClient::on_sendBtn_clicked()
{
    QString str_msg = ui->sendLine->text();
    if(!str_msg.isEmpty()){ //输入非空
        PDU * pdu = mkPDU(str_msg.size());
        pdu->ui_msg_type = 8888;
        memcpy(pdu->file_name,str_msg.toStdString().c_str(),str_msg.size());
        m_tcpSocket.write((char *)pdu,pdu->ui_pdu_len);
        free(pdu);
        pdu = NULL;
    }
    else{ //输入为空则给出提示信息
        QMessageBox::warning(this,"信息发送","发送信息不能为空！");
    }
}

