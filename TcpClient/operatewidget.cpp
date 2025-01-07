#include "operatewidget.h"
#include "tcpclient.h"

operateWidget::operateWidget(QWidget *parent)
    : QWidget{parent}
{
    m_pListWidget = new QListWidget(this);
    QString name = TcpClient::getInstance().getLoginName();
    mLabel = new QLabel(name);
    m_pListWidget->addItem("好友");
    m_pListWidget->addItem("文件");

    m_pFriend = new Friend;
    m_pFileSystem = new FileSystem;

    m_pSW = new QStackedWidget;//每次只会显示一个控件
    m_pSW->addWidget(m_pFriend);//默认显示第一个窗口
    m_pSW->addWidget(m_pFileSystem);

    QHBoxLayout * pMain = new QHBoxLayout;
    pMain->addWidget(mLabel);
    pMain->addWidget(m_pListWidget);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    // 当前行发生变化，与页面切换的函数关联
    connect(m_pListWidget,SIGNAL(currentRowChanged(int)),m_pSW,SLOT(setCurrentIndex(int)));
}

operateWidget &operateWidget::getInstance()
{
    static operateWidget instance;
    return instance;
}

Friend *operateWidget::getFriend()
{
    return m_pFriend;
}

FileSystem *operateWidget::getFile()
{
    return m_pFileSystem;
}
