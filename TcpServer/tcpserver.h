#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMainWindow>
#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpServer;
}
QT_END_NAMESPACE

class TcpServer : public QMainWindow
{
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig(); // 加载套接字信息

private:
    Ui::TcpServer *ui;
    QString m_strIP;
    quint16 m_port;
};
#endif // TCPSERVER_H
