#include "mytcpsocket.h"
#include <QDebug>
#include "operatedb.h"
#include <QStringList>
#include "mytcpserver.h"
#include <QFileInfo>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    // 当接收到客户端数据，服务器会发送readyRead()信号
    connect(this,SIGNAL(readyRead()),this,(SLOT(recvMsg())));
    // 服务器处理tcpsocket连接断开后的情形
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));

    mIsUpload = false;
    mTimer = new QTimer;

    // 设置定时器来发送文件，防止粘包
    connect(mTimer,SIGNAL(timeout()),this,SLOT(sendFileToClient()));

}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::copy(QString sourceDir, QString targetDir)
{
    QDir dir;
    dir.mkdir(targetDir);

    dir.setPath(sourceDir);
    QFileInfoList fileInfoList = dir.entryInfoList(); //获取文件列表

    QString sourceTmp;
    QString targetTmp;
    for(int i = 0;i < fileInfoList.size();i++){
        //"."和".."文件夹不用复制，否则死循环
        if(fileInfoList[i].fileName() == "." || fileInfoList[i].fileName() == ".."){
            continue;
        }
        if(fileInfoList[i].isFile()){
            qDebug() << "fileInfoList.fileName() --> " << fileInfoList[i].fileName();
            sourceTmp = sourceDir + "/" + fileInfoList[i].fileName();
            targetTmp = targetDir + "/" + fileInfoList[i].fileName();
            QFile::copy(sourceTmp,targetTmp);
        }
        else{
            sourceTmp = sourceDir + "/" + fileInfoList[i].fileName();
            targetTmp = targetDir + "/" + fileInfoList[i].fileName();
            copy(sourceTmp,targetTmp);
        }
    }
}

void MyTcpSocket::recvMsg()
{
    if(!mIsUpload) //如果不需要上传文件
    {
        // qDebug() << this->bytesAvailable(); //获取接收到的数据大小
        uint ui_pdu_len = 0;
        this->read((char *)&ui_pdu_len,sizeof(uint)); //先读取uint大小的数据
        uint ui_msg_len = ui_pdu_len - sizeof(PDU); //实际消息的大小
        PDU *pdu = mkPDU(ui_msg_len);
        this->read((char *)pdu + sizeof(uint),ui_pdu_len - sizeof(uint)); //读取剩下的数据，从pdu后偏移sizeof(uint)个字节开始读取
        switch(pdu->ui_msg_type) //收到数据后判断消息类型
        {
        case ENUM_MSG_TYPE_REGIST_REQUEST://如果是注册请求，就存储用户名和密码
        {
            char usr_name[32] = {'\0'};
            char usr_passwd[32] = {'\0'};
            strncpy(usr_name,pdu->usr_data,32);
            strncpy(usr_passwd,pdu->usr_data+32,32);
            bool ret = operateDB::getInstance().handleRegist(usr_name,usr_passwd);
            PDU * respdu = mkPDU(0);
            respdu->ui_msg_type = ENUM_MSG_TYPE_REGIST_RESPOND; // 发送信息给客户端
            if(ret){ // 回复注册成功
                strcpy(respdu->usr_data,REGIST_OK);
                // 创建用户目录
                QDir dir;
                qDebug() << "create dir : " << dir.mkdir(QString("./%1").arg(usr_name));
            }
            else{ // 回复注册失败
                strcpy(respdu->usr_data,REGIST_FAIL);
            }
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            pdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST://服务端登录请求
        {
            char usr_name[32] = {'\0'};
            char usr_passwd[32] = {'\0'};
            strncpy(usr_name,pdu->usr_data,32);
            strncpy(usr_passwd,pdu->usr_data+32,32);
            bool ret = operateDB::getInstance().handleLogin(usr_name,usr_passwd);
            PDU * respdu = mkPDU(0);
            respdu->ui_msg_type = ENUM_MSG_TYPE_LOGIN_RESPOND; // 发送信息给客户端
            if(ret){ // 回复登录成功
                strcpy(respdu->usr_data,LOGIN_OK);
                m_strName = usr_name;
            }
            else{ // 回复登录失败
                strcpy(respdu->usr_data,LOGIN_FAIL);
            }
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            pdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ONLINE_USR_REQUEST://客户端请求查看当前所有用户
        {
            QStringList ret = operateDB::getInstance().handleOnlineUsr();
            uint ui_msg_len = ret.size() * 32;
            PDU * respdu = mkPDU(ui_msg_len);
            respdu->ui_msg_type = ENUM_MSG_TYPE_ONLINE_USR_RESPOND;
            for(int i = 0;i < ret.size();i++){
                memcpy((char*)(respdu->usr_data) + i * 32,ret.at(i).toStdString().c_str(),ret.at(i).size());
            }
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USR_REQUEST://客户端请求搜索特定用户
        {
            int ret = operateDB::getInstance().handleSearchUsr(pdu->usr_data);
            PDU * respdu = mkPDU(0);
            respdu->ui_msg_type = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
            if(ret == -1){
                strcpy(respdu->usr_data,SEARCH_USR_NONE);
            }
            else if(ret == 1){
                strcpy(respdu->usr_data,SEARCH_USR_ONLINE);
            }
            else{
                strcpy(respdu->usr_data,SEARCH_USR_OFFLINE);
            }
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST://客户端请求添加好友
        {
            char addName[32] = {'\0'};
            char myName[32] = {'\0'};
            strncpy(addName,pdu->usr_data,32);
            strncpy(myName,pdu->usr_data+32,32);
            int ret = operateDB::getInstance().handleAddFriend(addName,myName);
            PDU * respdu = mkPDU(0);
            if(ret == -1){// 用户不存在
                respdu->ui_msg_type = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->usr_data,UNKNOWN_ERROR);
                write((char*)respdu,respdu->ui_pdu_len);
            }
            else if(ret == 0){ //用户不在线
                respdu->ui_msg_type = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->usr_data,ADD_USR_OFFLINE);
                write((char*)respdu,respdu->ui_pdu_len);
            }
            else if(ret == 1){ //用户在线,那么需要转发请求
                respdu->ui_msg_type = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->usr_data,ADD_USR_ONLINE);
                write((char*)respdu,respdu->ui_pdu_len);
                strncpy(respdu->usr_data,pdu->usr_data,32);
                strncpy(respdu->usr_data+32,pdu->usr_data + 32,32);
                respdu->ui_msg_type = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
                MyTcpServer::getInstance().addUsrMsgSend(addName,respdu);
            }
            else if(ret == 2){ //两人已经是好友
                respdu->ui_msg_type = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->usr_data,ALREADY_FRIEND);
                write((char*)respdu,respdu->ui_pdu_len);
            }
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE://客户端同意添加好友
        {
            char addName[32] = {'\0'};
            char myName[32] = {'\0'};
            // 拷贝读取的信息
            strncpy(addName, pdu -> usr_data, 32);
            strncpy(myName, pdu -> usr_data + 32, 32);
            PDU * respdu = mkPDU(0);
            // 将新的好友关系信息写入数据库
            operateDB::getInstance().handleAddFriendAgree(addName, myName);
            // 服务器需要转发给发送好友请求方其被同意的消息
            MyTcpServer::getInstance().addUsrMsgSend(myName, pdu);
            //write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REJECT://客户端不同意添加好友
        {
            char myName[32] = {'\0'};
            strncpy(myName, pdu -> usr_data + 32, 32);
            // 服务器需要转发给发送好友请求方其被拒绝的消息
            MyTcpServer::getInstance().addUsrMsgSend(myName, pdu);
            break;
        }
        case ENUM_MSG_TYPE_REFRESH_FRIEND_REQUEST://客户端请求刷新好友列表
        {
            char name[32] = {'\0'};
            strncpy(name, pdu -> usr_data, 32);
            // qDebug() << name;
            QStringList friendList = operateDB::getInstance().handleRefreshFriendList(name);
            uint ui_msg_len = friendList.size() * 32;
            PDU * respdu = mkPDU(ui_msg_len);
            respdu->ui_msg_type = ENUM_MSG_TYPE_REFRESH_FRIEND_RESPOND;
            for(int i = 0;i < friendList.size();i++){
                memcpy((char*)(respdu->usr_data) + i * 32,friendList.at(i).toStdString().c_str(),friendList.at(i).size());
            }
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST://客户端请求删除好友
        {
            char friendName[32] = {'\0'};
            char loginName[32] = {'\0'};
            strncpy(friendName, pdu -> usr_data, 32);
            strncpy(loginName, pdu -> usr_data + 32, 32);
            bool ret = operateDB::getInstance().handleDeleteFriend(friendName,loginName);
            // 给请求删除方消息提示，以返回值形式
            PDU *respdu = mkPDU(0);
            respdu -> ui_msg_type = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            if(ret)
            {
                strncpy(respdu -> usr_data, DELETE_FRIEND_OK, 32);
            }
            else
            {
                strncpy(respdu -> usr_data, DELETE_FRIEND_FAILED, 32);
            }
            // 给用户端返回消息
            write((char*)respdu, respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            // 给被删除方消息提示，如果在线的话
            MyTcpServer::getInstance().addUsrMsgSend(friendName, pdu);
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST://私聊请求
        {
            char chatName[32] = {'\0'};
            char sourceName[32] = {'\0'};
            // 拷贝读取的信息
            strncpy(chatName, pdu -> usr_data, 32);
            strncpy(sourceName, pdu -> usr_data + 32, 32);
            // qDebug() << "handlePrivateChatRequest  " << chatName << " " << sourceName;
            MyTcpServer::getInstance().addUsrMsgSend(chatName,pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST://群聊请求
        {
            char name[32] = {'\0'};
            strncpy(name, pdu -> usr_data, 32);
            QStringList friendList = operateDB::getInstance().handleRefreshFriendList(name);//查看当前用户有哪些好友在线
            for(int i = 0;i < friendList.size();i++){
                MyTcpServer::getInstance().addUsrMsgSend(friendList[i].toStdString().c_str(),pdu);//通知好友
            }
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST://创建文件夹的请求
        {
            QDir dir;
            QString curPath = QString("%1").arg((char *)pdu->msg);
            bool ret = dir.exists(curPath);
            PDU * respdu = NULL;
            if(ret){//当前目录已存在
                char newDir[32] = {'\0'};
                memcpy(newDir,pdu->usr_data + 32,32);
                QString newPath = curPath + "/" + newDir;
                ret = dir.exists(newPath);
                if(ret){//创建的文件名已经存在
                    respdu = mkPDU(0);
                    respdu->ui_msg_type = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->usr_data,FILENAME_EXIST);
                }
                else{//创建的文件名不存在
                    dir.mkdir(newPath);
                    respdu = mkPDU(0);
                    respdu->ui_msg_type = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->usr_data,CREATE_DIR_OK);
                }
            }
            else{//当前目录不存在
                respdu = mkPDU(0);
                respdu->ui_msg_type = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->usr_data,DIR_NO_EXISTANCE);
            }
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_REFRESH_DIR_REQUEST://查看文件夹请求
        {
            char * curPath = new char[pdu->ui_msg_len];
            memcpy(curPath,pdu->msg,pdu->ui_msg_len);
            QDir dir(curPath);
            QFileInfoList fileInfoList = dir.entryInfoList();//当前目录下所有文件
            int fileCount = fileInfoList.size();
            PDU * respdu = mkPDU(sizeof(FileInfo) * fileCount);
            respdu->ui_msg_type = ENUM_MSG_TYPE_REFRESH_DIR_RESPOND;
            FileInfo * fileInfo = NULL;
            QString fileName;
            for(int i = 0;i < fileCount;i++){
                qDebug() << fileInfoList[i].fileName() << " " << fileInfoList[i].size();
                fileInfo = (FileInfo*)(respdu->msg) + i;
                fileName = fileInfoList[i].fileName();
                memcpy(fileInfo->fileName,fileName.toStdString().c_str(),fileName.size());
                if(fileInfoList[i].isDir()){
                    fileInfo->fileType = 0;
                }
                else if(fileInfoList[i].isFile()){
                    fileInfo->fileType = 1;
                }
            }
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_DIR_REQUEST://删除文件夹请求
        {
            char fileName[32];
            strcpy(fileName,pdu->usr_data);
            char *path = new char[pdu->ui_msg_len];
            memcpy(path,pdu->msg,pdu->ui_msg_len);
            QString filePath = QString("%1/%2").arg(path).arg(fileName);
            // qDebug() << filePath;
            QFileInfo fileInfo(filePath);
            bool ret = false;
            if(fileInfo.isDir()){
                QDir dir;
                dir.setPath(filePath);
                ret = dir.removeRecursively();//删除当前目录及其所有内容
            }
            PDU * respdu = NULL;
            if(ret){//删除成功
                respdu = mkPDU(0);
                memcpy(respdu->usr_data,DEL_DIR_OK,strlen(DEL_DIR_OK));
            }
            else{//删除失败
                respdu = mkPDU(0);
                memcpy(respdu->usr_data,DEL_DIR_FAILURE,strlen(DEL_DIR_FAILURE));
            }
            respdu->ui_msg_type = ENUM_MSG_TYPE_DELETE_DIR_RESPOND;
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_REQUEST://重命名文件请求
        {
            char preName[32] = {'\0'};
            char newName[32] = {'\0'};
            strncpy(preName,pdu->usr_data,32);
            strncpy(newName,pdu->usr_data + 32,32);
            char *curPath = new char[pdu->ui_msg_len];
            memcpy(curPath,pdu->msg,pdu->ui_msg_len);
            QString prePath = QString("%1/%2").arg(curPath).arg(preName);//旧文件路径
            QString newPath = QString("%1/%2").arg(curPath).arg(newName);//新文件路径
            QDir dir;
            bool ret = dir.rename(prePath,newPath);//重命名
            qDebug() << ret;
            PDU * respdu = mkPDU(0);
            respdu->ui_msg_type = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
            if(ret){
                strncpy(respdu->usr_data,RENAME_FILE_OK,32);
            }
            else{
                strncpy(respdu->usr_data,RENAME_FILE_FAILURE,32);
            }
            qDebug() << respdu->usr_data;
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_REQUEST://进入文件夹请求
        {
            char enterDirName[32] = {'\0'};
            strncpy(enterDirName,pdu->usr_data,32);

            char * curPath = new char[pdu->ui_msg_len];
            memcpy(curPath,pdu->msg,pdu->ui_msg_len);

            QString path = QString("%1/%2").arg(curPath).arg(enterDirName);
            QFileInfo fileInfo(path);//判断当前文件类型
            PDU * respdu = NULL;
            if(fileInfo.isDir()){//dir可以进入后，执行和刷新文件相同的操作
                QDir dir(path);
                QFileInfoList fileInfoList = dir.entryInfoList();//当前目录下所有文件
                int fileCount = fileInfoList.size();
                respdu = mkPDU(sizeof(FileInfo) * fileCount);
                respdu->ui_msg_type = ENUM_MSG_TYPE_REFRESH_DIR_RESPOND;
                FileInfo * fileInfo = NULL;
                QString fileName;
                for(int i = 0;i < fileCount;i++){
                    qDebug() << fileInfoList[i].fileName() << " " << fileInfoList[i].size();
                    fileInfo = (FileInfo*)(respdu->msg) + i;
                    fileName = fileInfoList[i].fileName();
                    memcpy(fileInfo->fileName,fileName.toStdString().c_str(),fileName.size());
                    if(fileInfoList[i].isDir()){
                        fileInfo->fileType = 0;
                    }
                    else if(fileInfoList[i].isFile()){
                        fileInfo->fileType = 1;
                    }
                }
            }
            else if(fileInfo.isFile()){//普通文件无法进入
                respdu = mkPDU(0);
                respdu->ui_msg_type = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                strcpy(respdu->usr_data,ENTER_DIR_FAILURE);
                respdu->ui_msg_type = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
            }

            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST: //响应客户端上传文件的请求
        {
            char fileName[32] = {'\0'};
            qint64 fileSize = 0;
            sscanf(pdu->usr_data,"%s %lld",fileName,&fileSize);//依次读取文件名称和文件大小
            // qDebug() << "fileName --> " << fileName;
            // qDebug() << "fileSize --> " << fileSize;
            char * curPath = new char[pdu->ui_msg_len];
            // qDebug() << "curPath --> " << curPath;
            memcpy(curPath,pdu->msg,pdu->ui_msg_len);
            QString filePath = QString("%1/%2").arg(curPath).arg(fileName);//拼接文件路径和文件名
            // qDebug() << "filePath --> " << filePath;
            delete[] curPath;
            curPath = NULL;
            // 打开文件
            mFile.setFileName(filePath);
            //以只写的方式打开文件，若文件不存在则自动创建
            if(mFile.open(QIODevice::WriteOnly)){
                mIsUpload = true;//更新状态
                mFileSize = fileSize;//更新文件大小
                mFileRcvSize = 0;//上传的文件大小此时为0
            }
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FILE_REQUEST://删除常规文件请求
        {
            char fileName[32];
            strcpy(fileName,pdu->usr_data);
            char *path = new char[pdu->ui_msg_len];
            memcpy(path,pdu->msg,pdu->ui_msg_len);
            QString filePath = QString("%1/%2").arg(path).arg(fileName);
            // qDebug() << filePath;
            QFileInfo fileInfo(filePath);
            bool ret = false;
            if(fileInfo.isFile()){
                QDir dir;
                ret = dir.remove(filePath);//删除文件
            }
            PDU * respdu = NULL;
            if(ret){//删除成功
                respdu = mkPDU(0);
                memcpy(respdu->usr_data,DEL_FILE_OK,strlen(DEL_FILE_OK));
            }
            else{//删除失败
                respdu = mkPDU(0);
                memcpy(respdu->usr_data,DEL_FILE_FAILURE,strlen(DEL_FILE_FAILURE));
            }
            respdu->ui_msg_type = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST://下载文件请求
        {
            char fileName[32] = {'\0'};
            strcpy(fileName,pdu->usr_data);
            char * curPath = new char[pdu->ui_msg_len];
            memcpy(curPath,pdu->msg,pdu->ui_msg_len);
            QString filePath = QString("%1/%2").arg(curPath).arg(fileName);//拼接文件路径和文件名
            qDebug() <<"filePath --> " << filePath;
            delete[] curPath;
            curPath = NULL;
            QFileInfo fileInfo(filePath);
            qint64 fileSize = fileInfo.size();
            PDU * respdu = mkPDU(0);
            sprintf(respdu->usr_data,"%s %lld",fileName,fileSize);
            respdu->ui_msg_type = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            mFile.setFileName(filePath);
            bool ret = mFile.open(QIODevice::ReadOnly);//以只读方式打开文件
            qDebug() << "ret --> " << ret;
            mTimer->start(1000);//1s之后开始发送文件
            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_REQUEST://移动文件请求
        {
            char fileName[32] = {'\0'};
            int srcLen = 0;
            int targetLen = 0;
            sscanf(pdu->usr_data,"%d %d %s",&srcLen,&targetLen,fileName);
            char *sourcePath = new char[srcLen + 1];
            char *targetPath = new char[targetLen + 1 + 32];
            memset(sourcePath,'\0',srcLen + 1);
            memset(targetPath,'\0',srcLen + 1 + 32);
            memcpy(sourcePath,pdu->msg,srcLen);
            memcpy(targetPath,(char*)(pdu->msg) + (srcLen + 1),targetLen);
            PDU * respdu = mkPDU(0);
            respdu->ui_msg_type = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
            QFileInfo fileInfo(targetPath);
            if(fileInfo.isFile()){//如果这是一个文件夹
                strcpy(respdu->usr_data,MOVE_FILE_FAILURE);
            }
            else{//如果只是一个常规文件
                // 拼接得到存放移动文件的路径
                strcat(targetPath,"/");
                strcat(targetPath,fileName);
                bool ret = QFile::rename(sourcePath,targetPath);//移动
                if(ret){
                    strcpy(respdu->usr_data,MOVE_FILE_OK);
                }
                else{
                    strcpy(respdu->usr_data,COMMON_ERROR);
                }
            }
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST://共享文件请求
        {
            char senderName[32] = {'\0'};
            int num = 0;
            sscanf(pdu->usr_data,"%s%d",senderName,&num);//依次读取发送文件用户名、被分享文件的人数
            int size = num * 32;
            // 给目标用户转发消息，询问其是否接收共享文件
            PDU * notepdu = mkPDU(pdu->ui_msg_len - size);
            notepdu->ui_msg_type = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
            strcpy(notepdu->usr_data,senderName);//写入请求分享文件的人
            memcpy(notepdu->msg,(char*)(pdu->msg) + size,pdu->ui_msg_len - size);//文件所在的路径
            char targetName[32] = {'\0'};
            for(int i = 0;i < num;i++){
                memcpy(targetName,(char*)(pdu->msg) + i * 32,32);//提取目标用户名
                MyTcpServer::getInstance().addUsrMsgSend(targetName,notepdu);//将消息转发给目标用户
            }
            free(notepdu);
            notepdu = NULL;
            // 回复发送方消息
            PDU * respdu = mkPDU(0);
            respdu->ui_msg_type = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            strcpy(respdu->usr_data,SHARE_FILE_RESPOND);
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_AGREE://用户同意分享文件
        {
            QString rcvPath = QString("./%1").arg(pdu->usr_data);
            QString shareFilePath = QString("%1").arg((char*)pdu->msg);
            int index = shareFilePath.lastIndexOf('/');
            QString fileName = shareFilePath.right(shareFilePath.size() - index - 1);
            rcvPath = rcvPath + "/" + fileName; //共享文件的新路径
            // 拷贝文件 -- 收到判断这是文件夹还是常规文件
            QFileInfo fileInfo(shareFilePath);
            if(fileInfo.isFile()){
                QFile::copy(shareFilePath,rcvPath);
            }
            else{
                copy(shareFilePath,rcvPath);
            }
            break;
        }
        case ENUM_MSG_TYPE_LOGOUT_REQUEST://请求注销
        {
            char usr_name[32] = {'\0'};
            char usr_passwd[32] = {'\0'};
            strncpy(usr_name,pdu->usr_data,32);
            strncpy(usr_passwd,pdu->usr_data+32,32);
            bool ret = operateDB::getInstance().handleLogout(usr_name,usr_passwd);
            PDU * respdu = mkPDU(0);
            respdu->ui_msg_type = ENUM_MSG_TYPE_LOGOUT_RESPOND; // 发送信息给客户端
            if(ret){ // 回复注销成功
                strcpy(respdu->usr_data,LOGOUT_OK);
                // 删除用户目录
                QDir dir;
                dir.setPath(QString("./%1").arg(usr_name));
                dir.removeRecursively();
            }
            else{ // 回复注销失败
                strcpy(respdu->usr_data,LOGOUT_FAIL);
            }
            write((char*)respdu,respdu->ui_pdu_len);
            free(respdu);
            pdu = NULL;
            break;
        }
        default:
            break;
        }
        free(pdu);
        pdu = NULL;
    }
    else{
        this->setReadBufferSize(1024 * 1024);
        PDU *respdu = mkPDU(0);
        respdu->ui_msg_type = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
        // 写入文件
        QByteArray buff = readAll();
        mFile.write(buff);
        mFileRcvSize += buff.size();
        // 读取文件内容完成
        if(mFileSize == mFileRcvSize)
        {
            mIsUpload = false;
            mFile.close();
            strcpy(respdu->usr_data, UPLOAD_FILE_OK);
            write((char*)respdu, respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
        }
        // 读取数据错误的情况下不再读取了
        else if(mFileSize < mFileRcvSize)
        {
            mIsUpload = false;
            mFile.close();
            strcpy(respdu->usr_data, UPLOAD_FILE_FAILURE);
            write((char*)respdu, respdu->ui_pdu_len);
            free(respdu);
            respdu = NULL;
        }
        return ;
    }
}

void MyTcpSocket::clientOffline()
{
    operateDB::getInstance().handleOffline(m_strName.toStdString().c_str());// 修改数据库的状态信息
    emit offline(this);//发送删除信号

}

void MyTcpSocket::sendFileToClient()
{
    qDebug() << "开始发送文件！";
    mTimer->stop();
    char * fileData = new char[4096];
    qint64 readSize = 0;
    qint64 totalSize = 0;
    while(true){
        readSize = mFile.read(fileData,4096);
        totalSize += readSize;
        if(readSize > 0 && readSize <= 4096){
            write(fileData,readSize);
        }
        else if(readSize == 0){//文件发送完毕
            qDebug() << "文件发送完成！";
            mFile.close();
            break;
        }
        else{
            qDebug() << "文件发送失败！";
            mFile.close();
            break;
        }
    }
    qDebug() << "sendTotalSize --> " << totalSize;
    delete[] fileData;
    fileData = NULL;
    mFile.setFileName("");
}
