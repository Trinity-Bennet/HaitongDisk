#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpClient;
}
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig(); // 加载套接字信息
public slots:
    void showConnect(); // 显示连接状态

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_port;
    // 连接服务器，和服务器进行数据交互
    QTcpSocket m_tcpSocket;
};
#endif // TCPCLIENT_H
