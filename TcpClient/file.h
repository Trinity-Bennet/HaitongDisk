#ifndef FILE_H
#define FILE_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class File : public QWidget
{
    Q_OBJECT
public:
    explicit File(QWidget *parent = nullptr);

signals:
public slots:
private:
    QListWidget * mBookListW;
    QPushButton *mReturnBtn;//返回
    QPushButton *mCreateDirBtn;//创建文件夹
    QPushButton *mDelDirBtn;//删除文件夹
    QPushButton *mRenameDirBtn;//重命名文件夹
    QPushButton *mRefreshDirBtn;//刷新文件夹
    QPushButton *mUploadFileBtn;//上传文件
    QPushButton *mDelFileBtn;//删除文件
    QPushButton *mDownloadFileBtn;//下载文件
    QPushButton *mShareFileBtn;//分享文件
};

#endif // FILE_H
