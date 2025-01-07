#include "filesystem.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "operatewidget.h"
#include "sharefile.h"

FileSystem::FileSystem(QWidget *parent)
    : QWidget{parent}
{
    mEnterDir.clear();

    mIsDownloadFile = false;

    mTimer = new QTimer;

    mFileListW = new QListWidget;
    mReturnBtn = new QPushButton("返回上一级");//返回
    mCreateDirBtn = new QPushButton("创建文件夹");//创建文件夹
    mDelDirBtn = new QPushButton("删除文件夹");//删除文件夹
    mRenameDirBtn = new QPushButton("重命名文件");//重命名文件夹
    mRefreshDirBtn = new QPushButton("刷新文件");//刷新文件夹
    mUploadFileBtn = new QPushButton("上传文件");//上传文件
    mDelFileBtn = new QPushButton("删除文件");//删除文件
    mDownloadFileBtn = new QPushButton("下载文件");//下载文件
    mMoveFileBtn = new QPushButton("移动文件");//移动文件
    mShareFileBtn = new QPushButton("分享文件");//分享文件
    mSelectDirBtn = new QPushButton("选择目录");//选择目录
    mSelectDirBtn->setEnabled(false);

    QVBoxLayout *dirLayout = new QVBoxLayout;
    dirLayout->addWidget(mReturnBtn);
    dirLayout->addWidget(mRefreshDirBtn);
    dirLayout->addWidget(mCreateDirBtn);
    dirLayout->addWidget(mDelDirBtn);
    dirLayout->addWidget(mRenameDirBtn);

    QVBoxLayout *fileLayout = new QVBoxLayout;
    fileLayout->addWidget(mUploadFileBtn);
    fileLayout->addWidget(mDelFileBtn);
    fileLayout->addWidget(mDownloadFileBtn);
    fileLayout->addWidget(mMoveFileBtn);
    fileLayout->addWidget(mSelectDirBtn);
    fileLayout->addWidget(mShareFileBtn);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(mFileListW);
    hBoxLayout->addLayout(dirLayout);
    hBoxLayout->addLayout(fileLayout);

    setLayout(hBoxLayout);

    connect(mCreateDirBtn,SIGNAL(clicked(bool)),this,SLOT(createDir()));//创建文件夹按钮关联的事件
    connect(mRefreshDirBtn,SIGNAL(clicked(bool)),this,SLOT(refreshDir()));//查看文件夹按钮关联的事件
    connect(mDelDirBtn,SIGNAL(clicked(bool)),this,SLOT(deleteDir()));//删除文件夹按钮关联的事件
    connect(mRenameDirBtn,SIGNAL(clicked(bool)),this,SLOT(renameFile()));//重命名文件夹按钮关联的事件
    connect(mFileListW,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(enterDir()));//双击文件夹就进入
    connect(mReturnBtn,SIGNAL(clicked(bool)),this,SLOT(returnLastDir()));//返回上一级按钮关联的事件
    connect(mUploadFileBtn,SIGNAL(clicked(bool)),this,SLOT(uploadFile()));//上传文件按钮关联的事件
    connect(mTimer,SIGNAL(timeout()),this,SLOT(uploadFileData()));//隔一段时间才上传文件，防止粘包
    connect(mDelFileBtn,SIGNAL(clicked(bool)),this,SLOT(deleteFile()));//删除文件按钮关联的事件
    connect(mDownloadFileBtn,SIGNAL(clicked(bool)),this,SLOT(downloadFile()));//删除文件按钮关联的事件
    connect(mMoveFileBtn,SIGNAL(clicked(bool)),this,SLOT(moveFile()));//移动文件按钮关联的事件
    connect(mSelectDirBtn,SIGNAL(clicked(bool)),this,SLOT(selectTargetDir()));//选择目录按钮关联的事件
    connect(mShareFileBtn,SIGNAL(clicked(bool)),this,SLOT(shareFile()));//分享文件按钮关联的事件
}

void FileSystem::updateFileList(const PDU *pdu)
{
    //qDebug() << "这是更新文件夹的函数！";
    if(pdu == NULL){
        return;
    }
    mFileListW -> clear(); // 清除文件列表原有数据
    FileInfo * fileInfo = NULL;
    int count = pdu->ui_msg_len / sizeof(FileInfo);
    for(int i = 0;i < count;i++){
        fileInfo = (FileInfo*)(pdu->msg) + i;
        QListWidgetItem * item = new QListWidgetItem;//给文件设置图标
        if(fileInfo->fileType == 0){
            item->setIcon(QIcon(QPixmap(":/dir_icon.jpg")));
        }
        else{
            item->setIcon(QIcon(QPixmap(":/file_icon.jpg")));
        }
        item->setText(fileInfo->fileName);
        mFileListW->addItem(item);
        qDebug() << fileInfo->fileName << " " << fileInfo->fileType;
    }
}

void FileSystem::clearEnterDir()
{
    mEnterDir.clear();
}

QString FileSystem::enterCurDir()
{
    return mEnterDir;
}

void FileSystem::setDownloadFlag(bool status)
{
    mIsDownloadFile = status;
}

bool FileSystem::getDownloadFileStatus()
{
    return mIsDownloadFile;
}

QString FileSystem::getSaveFilePath()
{
    return mSaveFilePath;
}

QString FileSystem::getSharedFileName()
{
    return mSharedFileName;
}

void FileSystem::createDir()
{
    QString dir = QInputDialog::getText(this,"新建文件夹","新建文件夹名称");
    if(dir.isEmpty()){
        QMessageBox::warning(this,"新建文件夹","新建文件夹名称为空！");
        return;
    }
    QString name = TcpClient::getInstance().getLoginName();
    QString curPath = TcpClient::getInstance().getCurPath();
    PDU * pdu = mkPDU(curPath.size() + 1);
    pdu->ui_msg_type = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
    strncpy(pdu->usr_data,name.toStdString().c_str(),32);
    strncpy(pdu->usr_data + 32,dir.toStdString().c_str(),dir.size());
    memcpy(pdu->msg,curPath.toStdString().c_str(),curPath.size());
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

void FileSystem::refreshDir()
{
    QString curPath = TcpClient::getInstance().getCurPath();
    PDU * pdu = mkPDU(curPath.size() + 1);
    pdu->ui_msg_type = ENUM_MSG_TYPE_REFRESH_DIR_REQUEST;
    strncpy((char*)(pdu->msg),curPath.toStdString().c_str(),curPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

void FileSystem::deleteDir()
{
    QString curPath = TcpClient::getInstance().getCurPath();//获取当前目录
    QListWidgetItem * item = mFileListW->currentItem();//点击要删除的文件名
    if(item == NULL){
        QMessageBox::warning(this,"删除文件","请选择要删除的文件！");
        return;
    }
    QString deleteFileName = item->text();//获取要删除的文件名
    PDU * pdu = mkPDU(curPath.size() + 1);
    pdu->ui_msg_type = ENUM_MSG_TYPE_DELETE_DIR_REQUEST;
    strncpy(pdu->usr_data,deleteFileName.toStdString().c_str(),deleteFileName.size());//将要删除的文件名拷贝到usr_data中
    strncpy((char*)pdu->msg,curPath.toStdString().c_str(),curPath.size());//把路径放到msg中
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

void FileSystem::renameFile()
{
    QString curPath = TcpClient::getInstance().getCurPath();//获取当前目录
    QListWidgetItem * item = mFileListW->currentItem();//点击要重命名的文件名
    if(item == NULL){
        QMessageBox::warning(this,"重命名文件","请选择要重命名的文件！");
        return;
    }
    QString preFileName = item->text();//修改前的文件名
    QString newFileName = QInputDialog::getText(this,"重命名文件","请输入新的文件名");//获取新的文件名
    if(newFileName.isEmpty()){
        QMessageBox::warning(this,"重命名文件","新的文件名称不能为空！");
        return;
    }
    PDU * pdu = mkPDU(curPath.size() + 1);
    pdu->ui_msg_type = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
    // 将修改前后的文件名写入usr_data
    strncpy(pdu->usr_data,preFileName.toStdString().c_str(),preFileName.size());
    strncpy(pdu->usr_data + 32,newFileName.toStdString().c_str(),newFileName.size());
    // 将路径拷贝到msg中
    memcpy(pdu->msg,curPath.toStdString().c_str(),curPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

void FileSystem::enterDir()
{
    QListWidgetItem * item = mFileListW->currentItem();//点击要进入的文件名
    // qDebug() << item->text();
    QString dirName = item->text();
    mEnterDir = dirName;
    QString curPath = TcpClient::getInstance().getCurPath();//获取当前所在目录
    PDU *pdu = mkPDU(curPath.size() + 1);
    pdu->ui_msg_type = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    strncpy(pdu->usr_data,dirName.toStdString().c_str(),dirName.size());
    memcpy(pdu->msg,curPath.toStdString().c_str(),curPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

void FileSystem::returnLastDir()
{
    QString curPath = TcpClient::getInstance().getCurPath();//获取当前目录
    QString rootPath = "./" + TcpClient::getInstance().getLoginName();//根目录
    if(curPath == rootPath){//根目录不能再往上返回
        QMessageBox::warning(this,"返回上一级","已经在最开始的文件夹目录中！");
        return;
    }
    // 返回上一级 -- "./aa/bb/cc" --> "./aa/bb"
    uint splitIndex = curPath.lastIndexOf('/');//从后面往前面找/,返回/的位置
    curPath.remove(splitIndex,curPath.size() - splitIndex);//将路径中/及以后的字符全部删除
    TcpClient::getInstance().setCurPath(curPath);//设置新路径
    //  qDebug() << "return --> " << curPath;
    clearEnterDir();
    refreshDir();//给服务器发送刷新文件夹的请求
}

void FileSystem::uploadFile()
{
    QString curPath = TcpClient::getInstance().getCurPath();//获取当前目录
    mUploadFilePath = QFileDialog::getOpenFileName();//得到待上传文件的绝对路径
    qDebug() << mUploadFilePath;
    if(mUploadFilePath.isEmpty()){//文件的路径不能为空，否则提示
        QMessageBox::warning(this,"上传文件","上传的文件不能为空！");
        return;
    }
    //绝对路径中提取文件名，服务器以此作为新建文件的名称
    int index = mUploadFilePath.lastIndexOf('/');
    QString fileName = mUploadFilePath.right(mUploadFilePath.size() - index - 1);
    qDebug() << fileName;
    //获得文件大小
    QFile file(mUploadFilePath);
    qint64 fileSize = file.size();

    PDU * pdu = mkPDU(curPath.size() + 1);
    pdu->ui_msg_type = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
    memcpy(pdu->msg,curPath.toStdString().c_str(),curPath.size());//当前目录放入msg
    sprintf(pdu -> usr_data, "%s %lld", fileName.toStdString().c_str(), fileSize);//文件名和文件大小依次写入usr_data中
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
    mTimer->start(1000); //1000ms = 1s之后发出信号
}

void FileSystem::uploadFileData()
{
    mTimer->stop();//关闭定时器
    QFile file(mUploadFilePath);
    //以只读方式打开文件
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"上传文件","打开文件失败！");
        return;
    }
    char *buffer = new char[4096];
    qint64 totalSize = 0;
    qint32 count = 0;
    qint64 readSize = 0; //实际读取文件大小
    while(true){
        readSize = file.read(buffer,4096);//最多读取4096字节
        totalSize += readSize;
        if(readSize > 0 && readSize <= 4096){//读到了数据就发给服务器
            TcpClient::getInstance().getTcpSocket().write(buffer,readSize);
            count++;
        }
        else if(readSize == 0){//数据发送完毕
            break;
        }
        else{
            QMessageBox::warning(this,"上传文件","上传文件失败：读文件失败！");//报错提示
            break;
        }
    }
    qDebug() << "totalSize --> " << totalSize;
    qDebug() << "count --> " << count;
    file.close();
    delete[] buffer;
    buffer = NULL;
    mUploadFilePath.clear();
}

void FileSystem::deleteFile()
{
    QString curPath = TcpClient::getInstance().getCurPath();//获取当前目录
    QListWidgetItem * item = mFileListW->currentItem();//点击要删除的文件名
    if(item == NULL){
        QMessageBox::warning(this,"删除文件","请选择要删除的文件！");
        return;
    }
    QString deleteFileName = item->text();//获取要删除的文件名
    PDU * pdu = mkPDU(curPath.size() + 1);
    pdu->ui_msg_type = ENUM_MSG_TYPE_DELETE_FILE_REQUEST;
    strncpy(pdu->usr_data,deleteFileName.toStdString().c_str(),deleteFileName.size());//将要删除的文件名拷贝到usr_data中
    strncpy((char*)pdu->msg,curPath.toStdString().c_str(),curPath.size());//把路径放到msg中
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

void FileSystem::downloadFile()
{

    QString curPath = TcpClient::getInstance().getCurPath();//获取当前目录
    QListWidgetItem * item = mFileListW->currentItem();//点击要下载的文件名
    if(item == NULL){
        QMessageBox::warning(this,"下载文件","请选择要下载的文件！");
        return;
    }
    QString saveFilePath = QFileDialog::getSaveFileName();//下载文件的保存路径
    if(saveFilePath.isEmpty()){
        QMessageBox::warning(this,"下载文件","文件必须指定本地的保存位置！");
        saveFilePath.clear();
        return;
    }
    mSaveFilePath = saveFilePath;
    mDownloadFileRcvSize = 0;
    qDebug() << "保存路径为：" << mSaveFilePath;

    PDU * pdu = mkPDU(curPath.size() + 1);
    pdu->ui_msg_type = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
    QString fileName = item->text();
    strncpy(pdu->usr_data,fileName.toStdString().c_str(),fileName.size());
    memcpy(pdu->msg,curPath.toStdString().c_str(),curPath.size());
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

void FileSystem::moveFile()
{
    QListWidgetItem * curItem = mFileListW->currentItem();
    if(curItem == NULL){
        QMessageBox::warning(this,"移动文件","请选择需要移动的文件！");
        return;
    }
    mMoveFileName = curItem->text();//获取被移动的文件名
    QString curPath = TcpClient::getInstance().getCurPath();//获取当前文件所在目录
    mMoveFileCurPath = curPath + '/' + mMoveFileName;//获取当前文件的路径
    qDebug() << "移动文件的路径 --> " << mMoveFileCurPath;
    mSelectDirBtn->setEnabled(true);
}

void FileSystem::selectTargetDir()
{
    QListWidgetItem * curItem = mFileListW->currentItem();
    if(curItem == NULL){
        QMessageBox::warning(this,"移动文件","请选择移动文件的目标目录！");
        return;
    }
    QString targetDir = curItem->text();//获取目录名
    QString curPath = TcpClient::getInstance().getCurPath();//当前目录
    mTargetDir = curPath + '/' + targetDir;//移动文件的目标目录的路径
    qDebug() << "移动文件的目标目录 --> " << mTargetDir;
    mSelectDirBtn->setEnabled(false);

    int sourceLen = mMoveFileCurPath.size();
    int targetLen = mTargetDir.size();
    PDU * pdu = mkPDU(sourceLen + targetLen + 2);
    pdu->ui_msg_type = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    // 把当前路径大小、目标路径大小、移动文件名依次写入usr_data中
    sprintf(pdu->usr_data,"%d %d %s",sourceLen,targetLen,mMoveFileName.toStdString().c_str());
    // 移动文件的当前路径和目标目录的路径写入msg中
    memcpy(pdu->msg,mMoveFileCurPath.toStdString().c_str(),sourceLen);
    memcpy((char *)(pdu->msg) + sourceLen + 1,mTargetDir.toStdString().c_str(),targetLen);

    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

void FileSystem::shareFile()
{
    QString curPath = TcpClient::getInstance().getCurPath();//获取当前目录
    QListWidgetItem * item = mFileListW->currentItem();//点击要分享的文件名
    if(item == NULL){
        QMessageBox::warning(this,"分享文件","请选择要分享的文件！");
        return;
    }
    mSharedFileName = item->text();//获取文件名
    Friend *pFriend = operateWidget::getInstance().getFriend();
    QListWidget *pFriendList = pFriend->getFriendList();
    ShareFile::getInstance().updateFriend(pFriendList);//更新分享文件界面的好友列表
    if(ShareFile::getInstance().isHidden()){
        ShareFile::getInstance().show();//显示勾选框
    }
}
