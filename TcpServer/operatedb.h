#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class operateDB : public QObject
{
    Q_OBJECT
public:
    explicit operateDB(QObject *parent = nullptr);
    static operateDB& getInstance();
    void init(); //打开数据库连接
    ~operateDB(); // 析构函数，关闭数据库连接

    bool handleRegist(const char * name,const char * passwd); //处理用户注册
    bool handleLogin(const char * name,const char * passwd); //处理用户登录
    bool handleOffline(const char * name);//处理用户离线后状态
    QStringList handleOnlineUsr(); //处理在线用户
    int handleSearchUsr(const char * name);//搜索用户状态
    int handleAddFriend(const char * friendName,char * const usrName);//处理好友添加
    void handleAddFriendAgree(const char * friendName,const char * myName);//用户同意添加好友
    QStringList handleRefreshFriendList(const char * name);//处理刷新好友
    bool handleDeleteFriend(const char * friendName,const char * loginName);//处理删除好友
    bool handleLogout(const char * name,const char * passwd); //处理用户注销

signals:
public slots:
private:
    QSqlDatabase m_db; //连接数据库
};

#endif // OPERATEDB_H
