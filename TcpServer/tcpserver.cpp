#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"

TcpServer::TcpServer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);

    loadConfig();

    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_port); // 监听客户端
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
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
