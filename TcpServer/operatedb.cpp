#include "operatedb.h"
#include <QDebug>
#include <QMessageBox>

operateDB::operateDB(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QMYSQL"); //连接的数据库类型
    init();
}

operateDB &operateDB::getInstance()
{
    static operateDB instance;
    return instance;
}

void operateDB::init()
{
    m_db.setHostName("localhost");
    m_db.setUserName("root");
    m_db.setPassword("111222");
    m_db.setDatabaseName("info");
    if(m_db.open()) // 数据库是否打开成功
    {
        QSqlQuery query;
        query.exec("select * from userInfo");
        // while(query.next())
        // {
        //     QString data = QString("%1, %2, %3, %4").arg(query.value(0).toString()).arg(query.value(1).toString())
        //     .arg(query.value(2).toString()).arg(query.value(3).toString());
        //     qDebug() << data;
        // }
    }
    else
    {
        QMessageBox::critical(NULL, "数据库打开", "数据库打开失败");
    }
}

operateDB::~operateDB()
{
    m_db.close(); //关闭数据库连接
}

bool operateDB::handleRegist(const char *name, const char *passwd)
{
    if(name == NULL || passwd == NULL){
        return false;
    }
    QString data = QString("insert into userinfo(name, pwd) values('%1', '%2')").arg(name).arg(passwd);
    QSqlQuery query;
    return query.exec(data);
}

bool operateDB::handleLogin(const char *name, const char *passwd)
{
    if(name == NULL || passwd == NULL){
        return false;
    }
    QString data = QString("select * from userinfo where name = '%1' and pwd = '%2' and isOnline = 0").arg(name).arg(passwd);
    QSqlQuery query;
    query.exec(data);
    // 每次调用next都会返回一条结果，并将结果放入query中，返回值为true
    if(query.next()){
        QString data1 = QString("update userinfo set isOnline = 1 where name = '%1' and pwd = '%2' and isOnline = 0").arg(name).arg(passwd);
        query.exec(data1);
        return true;
    }
    else{
        return false;
    }
}

bool operateDB::handleOffline(const char *name)
{
    if(name == NULL){
        return false;
    }
    // 更新当前用户的在线状态
    QString data = QString("update userinfo set isOnline = 0 where name = \'%1\'").arg(name);
    QSqlQuery query;
    return query.exec(data);
}

QStringList operateDB::handleOnlineUsr()
{
    QString data = QString("select name from userinfo where isOnline = 1");
    QSqlQuery query;
    query.exec(data);
    QStringList result;
    result.clear();
    while(query.next()){
        result.append(query.value(0).toString());
    }
    return result;
}

int operateDB::handleSearchUsr(const char *name)
{
    if(name == NULL){
        return -1;
    }
    QString data = QString("select * from userinfo where name = '%1'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if(query.next()){ // 如果存在，需要判断是否在线
        return query.value(3).toInt();
    }
    else{ // 不存在
        return -1;
    }
}

int operateDB::handleAddFriend(const char *friendName, char * const usrName)
{
    if(friendName == NULL || usrName == NULL){
        return -1;
    }
    // 先查询用户是否在线，在线就转发请求消息
    QString strQuery = QString("select * from friendinfo "
                               "where (id = (select id from userinfo where name = \'%1\') and "
                               "friendId = (select id from userinfo where name = \'%2\')) or "  // 好友是双向的，数据库只存了单向，注意是or关系
                               "(id = (select id from userinfo where name = \'%3\') and "
                               "friendId = (select id from userinfo where name = \'%4\'))")
                           .arg(usrName).arg(friendName).arg(friendName).arg(usrName);
    qDebug() << strQuery;
    QSqlQuery query;
    query.exec(strQuery);
    if(query.next())
    {
        return 2; // 双方已经是好友
    }
    else // 不是好友
    {
        return handleSearchUsr(friendName); // 查询对方，存在并在线返回1，存在不在线返回0，不存在该用户返回-1
    }

}

void operateDB::handleAddFriendAgree(const char *friendName, const char *myName)
{
    QString strSql = QString("select id from userinfo where name = '%1'").arg(friendName);
    QSqlQuery query;
    query.exec(strSql);
    query.next();
    int friendId = query.value(0).toInt();
    // 获取登录用户id
    strSql = QString("select id from userinfo where name = '%1'").arg(myName);
    query.exec(strSql);
    query.next();
    int loginId = query.value(0).toInt();
    // 新增好友
    strSql = QString("insert into friendinfo (id,friendId) values ('%1','%2'), ('%2','%1')").arg(friendId).arg(loginId);
    query.exec(strSql);

}

QStringList operateDB::handleRefreshFriendList(const char *name)
{
    QStringList friendList;
    friendList.clear();
    if(name == NULL){
        return friendList;
    }
    // qDebug() << "要删除好友的id为：" << name;
    QString strSql = QString("select id from userinfo where name = \'%1\'").arg(name);// 获取用户名对应的id
    QSqlQuery query;
    query.exec(strSql);
    qDebug() << query.next();
    int usrId = query.value(0).toInt();
    // qDebug() << "当前用户的id为：" << usrId;
    strSql = QString("select friendId from friendinfo where id = %1").arg(usrId); //获取该用户的所有好友
    query.exec(strSql);
    while(query.next())
    {
        int friendId = query.value(0).toInt();
        // qDebug() << "当前好友的id为：" << friendId;
        strSql = QString("select name from userinfo where id = %1 and isOnline = 1").arg(friendId);
        QSqlQuery nameQuery;
        nameQuery.exec(strSql);
        if(nameQuery.next())
        {
            // qDebug() << "当前好友的名称为：" << nameQuery.value(0);
            friendList.append(nameQuery.value(0).toString());
        }
    }
    return friendList;

}

bool operateDB::handleDeleteFriend(const char *friendName, const char *loginName)
{
    if(friendName == NULL || loginName == NULL){
        return false;
    }
    QString strSql = QString("select id from userinfo where name = '%1'").arg(friendName);//获取待删除用户的id
    QSqlQuery query;
    query.exec(strSql);
    query.next();
    int friendId = query.value(0).toInt();
    strSql = QString("select id from userinfo where name = '%1'").arg(loginName);//获取当前用户的id
    query.exec(strSql);
    query.next();
    int userId = query.value(0).toInt();
    strSql = QString("delete from friendinfo where (id = '%1' and friendId = '%2') or (id = '%2' and friendId = '%1')").arg(userId).arg(friendId);
    query.exec(strSql);
    return true;
}

bool operateDB::handleLogout(const char *name, const char *passwd)
{
    if(name == NULL || passwd == NULL){
        return false;
    }
    qDebug() << "name --> " << name;
    qDebug() << "passwd --> " << passwd;
    QString data1 = QString("delete from userinfo where name = '%1' and pwd = '%2'").arg(name).arg(passwd);//在用户信息表中删除该用户信息
    QSqlQuery query1;
    query1.exec(data1);
    // 获取该用户的id
    QString data2 = QString("select id from userinfo where name = '%1'").arg(name);
    QSqlQuery query2;
    query2.exec(data2);
    query2.next();
    int userId = query2.value(0).toInt();
    qDebug() << "userId --> " << userId;
    QSqlQuery query3;
    QString data3 = QString("delete from friendinfo where id = '%1' or friendId = '%1'").arg(userId);
    query3.exec(data3);
    return true;
}

