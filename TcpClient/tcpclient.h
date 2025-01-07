#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "operatewidget.h"

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
    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();
    QString getLoginName();
    QString getCurPath();
    void setCurPath(QString curPath);//重新设置当前路径

public slots:
    void showConnect(); // 显示连接状态
    void recvMsg(); // 接收服务器数据

private slots:

    void on_login_btn_clicked();

    void on_register_btn_clicked();

    void on_logoff_btn_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_port;
    // 连接服务器，和服务器进行数据交互
    QTcpSocket m_tcpSocket;
    QString m_strName; //客户端当前登录用户的名称
    QString mPath; //当前路径
    QFile mFile;//下载的文件
};
#endif // TCPCLIENT_H
