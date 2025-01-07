#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include <QTimer>

class FileSystem : public QWidget
{
    Q_OBJECT
public:
    explicit FileSystem(QWidget *parent = nullptr);
    void updateFileList(const PDU * pdu);//刷新文件列表
    void clearEnterDir();//清空当前目录
    QString enterCurDir();//进入到当前目录
    void setDownloadFlag(bool status);//设置为下载文件的状态
    bool getDownloadFileStatus();//获取下载文件的状态
    QString getSaveFilePath();//获取下载文件保存路径
    QString getSharedFileName();//获取被分享的文件名

    qint64 mDownloadFileSize;//下载文件的总大小
    qint64 mDownloadFileRcvSize;//已经下载的文件数据大小

signals:
public slots:
    void createDir(); //创建文件夹
    void refreshDir(); //刷新文件夹内容
    void deleteDir(); //删除文件夹
    void renameFile(); //重命名文件夹
    void enterDir(); //进入文件夹
    void returnLastDir(); //返回上一级目录
    void uploadFile(); //上传文件请求
    void uploadFileData();//上传文件
    void deleteFile(); //删除常规文件
    void downloadFile(); //下载文件
    void moveFile(); //移动文件
    void selectTargetDir();//移动文件选择目录
    void shareFile(); //分享文件
private:
    QListWidget * mFileListW;//文件列表
    QPushButton *mReturnBtn;//返回
    QPushButton *mCreateDirBtn;//创建文件夹
    QPushButton *mDelDirBtn;//删除文件夹
    QPushButton *mRenameDirBtn;//重命名文件夹
    QPushButton *mRefreshDirBtn;//刷新文件夹
    QPushButton *mUploadFileBtn;//上传文件
    QPushButton *mDelFileBtn;//删除文件
    QPushButton *mDownloadFileBtn;//下载文件
    QPushButton *mMoveFileBtn;//移动文件
    QPushButton *mShareFileBtn;//分享文件
    QPushButton *mSelectDirBtn;//选择目录

    QString mEnterDir;//保存当前目录
    QString mUploadFilePath;//上传文件的路径
    QTimer * mTimer;//定时器
    QString mSaveFilePath;//下载文件的保存位置
    bool mIsDownloadFile;//是否处于下载文件的状态
    QString mMoveFileName;//被移动的文件名称
    QString mMoveFileCurPath;//被移动文件的当前路径
    QString mTargetDir;//移动文件的目标目录
    QString mSharedFileName;//被分享的文件名
};

#endif // FILESYSTEM_H
