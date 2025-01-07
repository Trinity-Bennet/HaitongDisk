#include "file.h"

File::File(QWidget *parent)
    : QWidget{parent}
{
    mBookListW = new QListWidget;
    mReturnBtn = new QPushButton("返回");//返回
    mCreateDirBtn = new QPushButton("创建文件夹");//创建文件夹
    mDelDirBtn = new QPushButton("删除文件夹");//删除文件夹
    mRenameDirBtn = new QPushButton("重命名文件夹");//重命名文件夹
    mRefreshDirBtn = new QPushButton("刷新文件夹");//刷新文件夹
    mUploadFileBtn = new QPushButton("上传文件");//上传文件
    mDelFileBtn = new QPushButton("删除文件");//删除文件
    mDownloadFileBtn = new QPushButton("下载文件");//下载文件
    mShareFileBtn = new QPushButton("分享文件");//分享文件

    QVBoxLayout *dirLayout = new QVBoxLayout;
    dirLayout->addWidget(mReturnBtn);
    dirLayout->addWidget(mCreateDirBtn);
    dirLayout->addWidget(mDelDirBtn);
    dirLayout->addWidget(mRenameDirBtn);
    dirLayout->addWidget(mRefreshDirBtn);

    QVBoxLayout *fileLayout = new QVBoxLayout;
    fileLayout->addWidget(mUploadFileBtn);
    fileLayout->addWidget(mDelFileBtn);
    fileLayout->addWidget(mDownloadFileBtn);
    fileLayout->addWidget(mShareFileBtn);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addLayout(dirLayout);
    hBoxLayout->addLayout(fileLayout);
    hBoxLayout->addWidget(mBookListW);

}
