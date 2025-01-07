#ifndef OPERATEWIDGET_H
#define OPERATEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "friend.h"
#include "filesystem.h"
#include <QStackedWidget> // 每次只显示一个窗口
#include <QLabel>

class operateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit operateWidget(QWidget *parent = nullptr);
    static operateWidget &getInstance();
    Friend * getFriend();
    FileSystem * getFile();

signals:
public slots:
signals:
private:
    QLabel * mLabel;
    QListWidget * m_pListWidget;
    Friend * m_pFriend;
    FileSystem * m_pFileSystem;
    QStackedWidget * m_pSW;
};

#endif // OPERATEWIDGET_H
