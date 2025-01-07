#include "sharefile.h"
#include <QCheckBox>
#include "tcpclient.h"

ShareFile::ShareFile(QWidget *parent)
    : QWidget{parent}
{
    mSelectAllBtn = new QPushButton("全选"); //全选
    mCancelSelectionBtn = new QPushButton("取消选择"); //取消选择
    mCommitBtn = new QPushButton("确认"); //确认
    mCancelBtn = new QPushButton("取消"); //取消

    mFriendArea = new QScrollArea; //滚动条区域
    mFriendW = new QWidget; //在线好友窗口
    mFriendWVBL = new QVBoxLayout(mFriendW);//好友勾选框使用垂直布局
    mManageFriend = new QButtonGroup(mFriendW); //按钮组
    mManageFriend->setExclusive(false);//设置多选

    QHBoxLayout * mTopHLayout = new QHBoxLayout;//顶部按钮水平布局
    mTopHLayout->addWidget(mSelectAllBtn);
    mTopHLayout->addWidget(mCancelSelectionBtn);
    mTopHLayout->addStretch();//弹簧

    QHBoxLayout * mBottomHLayout = new QHBoxLayout;//底部按钮水平布局
    mBottomHLayout->addWidget(mCommitBtn);
    mBottomHLayout->addWidget(mCancelBtn);
    mBottomHLayout->addStretch();

    QVBoxLayout * mMainLayout = new QVBoxLayout;//整体垂直布局
    mMainLayout->addLayout(mTopHLayout);
    mMainLayout->addWidget(mFriendArea);
    mMainLayout->addLayout(mBottomHLayout);

    setLayout(mMainLayout);//设置布局

    connect(mCancelSelectionBtn,SIGNAL(clicked()),this,SLOT(cancelSelectFriend()));//取消选择按钮关联的事件
    connect(mSelectAllBtn,SIGNAL(clicked()),this,SLOT(selectAll()));//全选按钮关联的事件
    connect(mCommitBtn,SIGNAL(clicked()),this,SLOT(commitList()));//确认按钮关联的事件
    connect(mCancelBtn,SIGNAL(clicked()),this,SLOT(cancelList()));//取消按钮关联的事件
}

ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

void ShareFile::updateFriend(QListWidget *pFriendList)
{
    if(pFriendList == NULL){
        return;
    }
    // 获取并清空之前的好友列表
    QAbstractButton * tmp = NULL;
    QList<QAbstractButton*> preFriendList = mManageFriend->buttons(); //获取之前的QCheckBox
    for(int i = 0;i < preFriendList.size();i++){
        tmp = preFriendList[i];
        mFriendWVBL->removeWidget(tmp);//移除布局
        mManageFriend->removeButton(tmp);//移除按钮组
        preFriendList.removeOne(tmp);//列表中删除
        delete tmp;
        tmp = NULL;
    }
    // 向区域框中重写新的内容
    QCheckBox * pCB = NULL; //勾选或取消勾选的复选框
    for(int i = 0;i < pFriendList->count();i++){
        pCB = new QCheckBox(pFriendList->item(i)->text());//添加复选框
        mFriendWVBL->addWidget(pCB);//加入垂直布局
        mManageFriend->addButton(pCB);
    }
    mFriendArea->setWidget(mFriendW);
}

void ShareFile::cancelSelectFriend()
{
    QList<QAbstractButton*> curListButtons = mManageFriend->buttons(); //获得QCheckBox的指针列表
    for(QAbstractButton * item : curListButtons){
        item->setChecked(false);//遍历列表，把所有复选框的状态设置为取消勾选
    }
}

void ShareFile::selectAll()
{
    QList<QAbstractButton*> curListButtons = mManageFriend->buttons(); //获得QCheckBox的指针列表
    for(QAbstractButton * item : curListButtons){
        item->setChecked(true);//遍历列表，把所有复选框的状态设置为勾选
    }
}

void ShareFile::commitList()
{
    QString loginUsrName = TcpClient::getInstance().getLoginName();//获取当前分享文件的用户名
    QString curPath = TcpClient::getInstance().getCurPath();//获取当前路径
    // qDebug() << "curPath --> " << curPath;)
    QString sharedFileName = operateWidget::getInstance().getFile()->getSharedFileName();//获得共享的文件名
    QString filePath = curPath + "/" + sharedFileName;
    QList<QAbstractButton*> curListButtons = mManageFriend->buttons(); //获得QCheckBox的指针列表
    int count = 0; //记录需要分享文件的对象数量
    for(QAbstractButton * item : curListButtons){//获取要分享文件的好友人数
        if(item->isChecked()){
            count++;
        }
    }
    PDU * pdu = mkPDU(count * 32 + filePath.size() + 1);
    pdu->ui_msg_type = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    // 将当前用户名和分享文件的用户人数写入usr_data中
    sprintf(pdu->usr_data,"%s %d",loginUsrName.toStdString().c_str(),count);

    // 将被分享文件的用户名称写入msg中
    int j = 0;
    for(int i = 0;i < curListButtons.size();i++){
        if(curListButtons[i] -> isChecked()){
            memcpy((char*)(pdu->msg) + j * 32,curListButtons[i]->text().toStdString().c_str(),curListButtons[i]->text().size());
            j++;
        }
    }
    // 将被分享的文件路径写入msg中
    memcpy((char*)(pdu->msg) + count * 32,filePath.toStdString().c_str(),filePath.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->ui_pdu_len);
    free(pdu);
    pdu = NULL;
}

void ShareFile::cancelList()
{
    hide();
}
