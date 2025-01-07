#include "tcpclient.h"
#include "ui_tcpclient.h"
#include "protocol.h"
#include "operatewidget.h"

#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "privatechat.h"

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    ui->usr_passwd_text->setEchoMode(QLineEdit::Password); //输入框变成圆点

    loadConfig();

    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_port); // 连接服务器

    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));// 连接服务器成功后显示连接成功

    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));// 接收服务器端的数据
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

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::getLoginName()
{
    return m_strName;
}

QString TcpClient::getCurPath()
{
    return mPath;
}

void TcpClient::setCurPath(QString curPath)
{
    mPath = curPath;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功！");
}

void TcpClient::recvMsg()
{
    if(!operateWidget::getInstance().getFile()->getDownloadFileStatus()){
        // qDebug() << m_tcpSocket->bytesAvailable(); //获取接收到的数据大小
        uint ui_pdu_len = 0;
        m_tcpSocket.read((char *)&ui_pdu_len,sizeof(uint)); //先读取uint大小的数据
        uint ui_msg_len = ui_pdu_len - sizeof(PDU); //实际消息的大小
        PDU *pdu = mkPDU(ui_msg_len);
        m_tcpSocket.read((char *)pdu + sizeof(uint),ui_pdu_len - sizeof(uint)); //读取剩下的数据，从pdu后偏移sizeof(uint)个字节开始读取
        // qDebug() << "收到数据！";
        switch(pdu->ui_msg_type) //收到数据后判断消息类型
        {
        case ENUM_MSG_TYPE_REGIST_RESPOND://注册回复
        {
            if(strcmp(pdu->usr_data,REGIST_OK) == 0){
                QMessageBox::information(this,"注册","注册成功！");
            }
            else if(strcmp(pdu->usr_data,REGIST_FAIL) == 0){
                QMessageBox::warning(this,"注册","注册失败,用户已存在");
            }
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_RESPOND://登录回复
        {
            if(strcmp(pdu->usr_data,LOGIN_OK) == 0){
                mPath = QString("./%1").arg(m_strName);
                QMessageBox::information(this,"登录","登录成功！");
                operateWidget::getInstance().show();
                this->hide();
            }
            else if(strcmp(pdu->usr_data,LOGIN_FAIL) == 0){
                QMessageBox::warning(this,"登录","登录失败,用户信息错误！");
            }
            break;
        }
        case ENUM_MSG_TYPE_ONLINE_USR_RESPOND://查看用户的回复
        {
            operateWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USR_RESPOND://搜索用户的回复
        {
            if(strcmp(pdu->usr_data,SEARCH_USR_NONE) == 0){
                QMessageBox::information(this,"查找用户",QString("%1: does not exist!").arg(operateWidget::getInstance().getFriend()->m_strSearchName));
            }
            else if(strcmp(pdu->usr_data,SEARCH_USR_ONLINE) == 0){
                QMessageBox::information(this,"查找用户",QString("%1: online!").arg(operateWidget::getInstance().getFriend()->m_strSearchName));
            }
            else{
                QMessageBox::information(this,"查找用户",QString("%1: offline!").arg(operateWidget::getInstance().getFriend()->m_strSearchName));
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST://添加用户的请求
        {
            char name[32] = {'\0'};
            strncpy(name,pdu->usr_data + 32,32);
            qDebug() << name;
            int ret = QMessageBox::information(this, "添加好友", QString("'%1' want to add you as friend.").arg(name), QMessageBox::Yes, QMessageBox::No);
            PDU* resPdu = mkPDU(0);
            strncpy(resPdu->usr_data, pdu->usr_data, 32); // 被加好友者用户名
            strncpy(resPdu->usr_data + 32, pdu->usr_data + 32, 32); // 加好友者用户名
            if(ret == QMessageBox::Yes) // 同意加好友
            {
                resPdu->ui_msg_type = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
            }
            else
            {
                resPdu->ui_msg_type = ENUM_MSG_TYPE_ADD_FRIEND_REJECT;
            }
            m_tcpSocket.write((char*)resPdu, resPdu -> ui_pdu_len); // 发送给服务器消息，由服务器写入数据库并转发给用户
            free(resPdu);
            resPdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND://添加用户的回复
        {
            QMessageBox::information(this,"添加好友", pdu->usr_data);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE: // 对方同意加好友
        {
            QMessageBox::information(this, "添加好友", QString("%1 已同意您的好友申请！").arg(pdu -> usr_data));
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REJECT: // 对方拒绝加好友
        {
            QMessageBox::information(this, "添加好友", QString("%1 已拒绝您的好友申请！").arg(pdu -> usr_data));
            break;
        }
        case ENUM_MSG_TYPE_REFRESH_FRIEND_RESPOND: //刷新用户的回复
        {
            operateWidget::getInstance().getFriend()->updateFriendList(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST: //删除用户的通知
        {
            char sourceName[32]; // 获取发送方用户名
            strncpy(sourceName, pdu -> usr_data + 32, 32);
            QMessageBox::information(this, "删除好友", QString("%1 已将您从好友列表删除！").arg(sourceName));
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND: //删除用户的回复
        {
            QMessageBox::information(this, "删除好友", pdu -> usr_data);
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST: //用户私聊
        {
            if(PrivateChat::getInstance().isHidden()){
                PrivateChat::getInstance().show();
            }
            char loginName[32] = {'\0'};
            memcpy(loginName, pdu->usr_data + 32, 32);
            PrivateChat::getInstance().setChatName(loginName);
            PrivateChat::getInstance().updateMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST: //群聊通知
        {
            operateWidget::getInstance().getFriend()->updateGroupMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_RESPOND: //创建文件夹回复
        {
            QMessageBox::information(this,"创建文件",pdu->usr_data);
            break;
        }
        case ENUM_MSG_TYPE_REFRESH_DIR_RESPOND: //查看文件夹回复
        {
            //qDebug() << "收到查看文件夹的回复！";
            operateWidget::getInstance().getFile()->updateFileList(pdu);
            QString enterDir = operateWidget::getInstance().getFile()->enterCurDir();
            if(!enterDir.isEmpty()){
                mPath = mPath + "/" + enterDir;
                qDebug() << mPath;
                operateWidget::getInstance().getFile()->clearEnterDir();
            }
            break;
        }
        case ENUM_MSG_TYPE_DELETE_DIR_RESPOND: //删除文件夹回复
        {
            //qDebug() << "收到查看文件夹的回复！";
            QMessageBox::information(this, "删除文件夹", pdu -> usr_data);
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_RESPOND: //重命名文件夹回复
        {
            // qDebug() << "收到重命名文件夹的回复！";
            QMessageBox::information(this, "重命名文件", pdu -> usr_data);
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_RESPOND: //进入文件夹回复
        {
            operateWidget::getInstance().getFile()->clearEnterDir();
            QMessageBox::information(this, "进入文件夹", pdu -> usr_data);//进入下一级失败
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND: //上传文件的回复
        {
            QMessageBox::information(this,"上传文件",pdu->usr_data);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FILE_RESPOND: //删除文件的回复
        {
            QMessageBox::information(this,"删除文件",pdu->usr_data);
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND: //下载文件的回复
        {
            // qDebug() << "文件名 - 文件大小 ：" << pdu->usr_data;
            char fileName[32] = {'\0'};
            sscanf(pdu->usr_data,"%s %lld",fileName,&(operateWidget::getInstance().getFile()->mDownloadFileSize));
            qDebug() << "rcvFileName --> " << fileName;
            if(strlen(fileName) > 0 && operateWidget::getInstance().getFile()->mDownloadFileSize > 0){
                operateWidget::getInstance().getFile()->setDownloadFlag(true);//更新当前状态为下载
                mFile.setFileName(operateWidget::getInstance().getFile()->getSaveFilePath());//获得文件的保存路径
                qDebug() << operateWidget::getInstance().getFile()->getSaveFilePath();
                //以只写的方式打开文件
                if(!mFile.open(QIODevice::WriteOnly)){
                    QMessageBox::warning(this,"下载文件","没有获得文件保存的路径！");
                    return;
                }
             }
            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_RESPOND: //移动文件的回复
        {
            QMessageBox::information(this,"移动文件",pdu->usr_data);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPOND: //共享文件的回复
        {
            QMessageBox::information(this,"共享文件",pdu->usr_data);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE: //是否接收对方分享的文件
        {
            char * filePath = new char[pdu->ui_msg_len];
            memcpy(filePath,pdu->msg,pdu->ui_msg_len);
            qDebug() << "filePath --> " << filePath;
            char * pos = strrchr(filePath,'/');//从后面寻找字符
            if(pos != NULL){
                pos++;
                QString note = QString("%1 shares file : %2 \n Do you want to recieve it?").arg(pdu->usr_data).arg(pos);
                int ret = QMessageBox::question(this,"共享文件",note);
                if(ret == QMessageBox::Yes){
                    PDU * respdu = mkPDU(pdu->ui_msg_len);
                    respdu->ui_msg_type = ENUM_MSG_TYPE_SHARE_FILE_AGREE;
                    memcpy(respdu->msg,pdu->msg,pdu->ui_msg_len);
                    QString usrName = TcpClient::getInstance().getLoginName();
                    strncpy(respdu->usr_data,usrName.toStdString().c_str(),usrName.size());
                    m_tcpSocket.write((char*)respdu,respdu->ui_pdu_len);
                    free(respdu);
                    respdu = NULL;
                }
            }
            break;
        }
        case ENUM_MSG_TYPE_LOGOUT_RESPOND://注销回复
        {
            if(strcmp(pdu->usr_data,LOGOUT_OK) == 0){
                QMessageBox::information(this,"注销","注销成功！");
            }
            else if(strcmp(pdu->usr_data,LOGOUT_FAIL) == 0){
                QMessageBox::warning(this,"注销","注销失败");
            }
            break;
        }
        default:
            break;
        }
        free(pdu);
        pdu = NULL;
    }
    else{//如果是下载文件，则读取数据
        QByteArray buffer =  m_tcpSocket.readAll();
        mFile.write(buffer);
        qDebug() << "buffersize --> " << buffer.size();
        FileSystem * file = operateWidget::getInstance().getFile();
        file->mDownloadFileRcvSize += buffer.size();
        qDebug() << "rcvSize --> " << file->mDownloadFileRcvSize;
        if(file->mDownloadFileSize == file->mDownloadFileRcvSize){
            mFile.close();
            file->mDownloadFileRcvSize = 0;
            file->mDownloadFileSize = 0;
            file->setDownloadFlag(false);
            QMessageBox::information(this,"下载文件","下载文件成功！");
        }
        else if(file->mDownloadFileSize < file->mDownloadFileRcvSize){
            mFile.close();
            file->mDownloadFileRcvSize = 0;
            file->mDownloadFileSize = 0;
            file->setDownloadFlag(false);
            QMessageBox::warning(this,"下载文件","下载文件失败！");
        }
    }
}

// 登录按钮
void TcpClient::on_login_btn_clicked()
{
    QString usr_name = ui->usr_name_text->text();
    QString usr_passwd = ui->usr_passwd_text->text();
    if(!usr_name.isEmpty() && !usr_passwd.isEmpty()){ //用户名和密码都不为空
        m_strName = usr_name;
        PDU *pdu = mkPDU(0); //此处不需要使用传递消息内容
        pdu->ui_msg_type = ENUM_MSG_TYPE_LOGIN_REQUEST;
        // 数据库最多存放32位，此处也只拷贝32位
        memcpy(pdu->usr_data,usr_name.toStdString().c_str(),32);
        memcpy(pdu->usr_data + 32,usr_passwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->ui_pdu_len);
        free(pdu);
        pdu = NULL;
    }
    else{
        QMessageBox::warning(this,"登录失败","用户名或密码不能为空！");
    }
}

// 注册按钮
void TcpClient::on_register_btn_clicked()
{
    QString usr_name = ui->usr_name_text->text();
    QString usr_passwd = ui->usr_passwd_text->text();
    if(!usr_name.isEmpty() && !usr_passwd.isEmpty()){ //用户名和密码都不为空
        PDU *pdu = mkPDU(0); //此处不需要使用传递消息内容
        pdu->ui_msg_type = ENUM_MSG_TYPE_REGIST_REQUEST;
        // 数据库最多存放32位，此处也只拷贝32位
        memcpy(pdu->usr_data,usr_name.toStdString().c_str(),32);
        memcpy(pdu->usr_data + 32,usr_passwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->ui_pdu_len);
        free(pdu);
        pdu = NULL;
    }
    else{
         QMessageBox::warning(this,"注册失败","用户名或密码不能为空！");
    }
}

// 注销按钮
void TcpClient::on_logoff_btn_clicked()
{
    QString usr_name = ui->usr_name_text->text();
    QString usr_passwd = ui->usr_passwd_text->text();
    if(!usr_name.isEmpty() && !usr_passwd.isEmpty()){ //用户名和密码都不为空
        PDU *pdu = mkPDU(0); //此处不需要使用传递消息内容
        pdu->ui_msg_type = ENUM_MSG_TYPE_LOGOUT_REQUEST;
        // 数据库最多存放32位，此处也只拷贝32位
        memcpy(pdu->usr_data,usr_name.toStdString().c_str(),32);
        memcpy(pdu->usr_data + 32,usr_passwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->ui_pdu_len);
        free(pdu);
        pdu = NULL;
        //qDebug() << "注销消息已发送！";
    }
    else{
        QMessageBox::warning(this,"注销失败","用户名或密码不能为空！");
    }
}

