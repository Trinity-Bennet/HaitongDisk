#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include <QDir>
#include <QFileInfoList>
#include <QFile>
#include <QTimer>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getName();
    void copy(QString sourceDir,QString targetDir);//拷贝目录
signals:
    void offline(MyTcpSocket * m_socket);
public slots:
    void recvMsg();// 处理客户端按照协议传输的数据
    void clientOffline(); //处理用户下线后的设置
    void sendFileToClient();//服务端发送文件
private:
    QString m_strName;//判断当前socket属于哪个用户
    QFile mFile;
    qint64 mFileSize; //上传文件的总大小
    qint64 mFileRcvSize; //已经接收的文件大小
    bool mIsUpload; //是否处于上传文件的状态
    QTimer * mTimer;//定时器
};

#endif // MYTCPSOCKET_H
