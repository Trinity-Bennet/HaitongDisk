#ifndef PROTOCOL_H
#define PROTOCOL_H

#include<stdlib.h>
#include<unistd.h>
#include<string.h>

typedef unsigned int uint;
#define REGIST_OK "regist succeesfully"
#define REGIST_FAIL "regist failed -- name existed"
#define LOGIN_OK "login successfully"
#define LOGIN_FAIL "login failed -- name error;password error;relogin!"
#define SEARCH_USR_NONE "user does not exist"
#define SEARCH_USR_ONLINE "online"
#define SEARCH_USR_OFFLINE "offline"
#define UNKNOWN_ERROR "unknown error"
#define ADD_USR_OFFLINE "usr offline"
#define ADD_USR_ONLINE "usr online"
#define ALREADY_FRIEND "already friends"
#define DELETE_FRIEND_OK "delete friend successfully"
#define DELETE_FRIEND_FAILED "delete friend failed"
#define DIR_NO_EXISTANCE "dir does not exist!"
#define FILENAME_EXIST "file exists!"
#define CREATE_DIR_OK "create dir successfully!"
#define DEL_DIR_OK "delete dir successfully!"
#define DEL_DIR_FAILURE "You cannot delete a regular file!"
#define RENAME_FILE_OK "rename file successfully"
#define RENAME_FILE_FAILURE "rename file failed"
#define ENTER_DIR_FAILURE "This is a regular file!"
#define UPLOAD_FILE_OK "upload file successfully"
#define UPLOAD_FILE_FAILURE "upload file failed"
#define DEL_FILE_OK "delete file successfully!"
#define DEL_FILE_FAILURE "You cannot delete a directory!"
#define MOVE_FILE_OK "file moved!"
#define MOVE_FILE_FAILURE "failed : that is a regular file!"
#define COMMON_ERROR "system busy!"
#define SHARE_FILE_RESPOND "msg has been sent!"
#define LOGOUT_OK "logout successfully"
#define LOGOUT_FAIL "logout failed -- name error;password error;!"

enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUEST,//注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,//注册回复
    ENUM_MSG_TYPE_LOGIN_REQUEST,//登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,//登录回复
    ENUM_MSG_TYPE_ONLINE_USR_REQUEST,//查看在线用户请求
    ENUM_MSG_TYPE_ONLINE_USR_RESPOND,//针对查看在线用户请求的回复
    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,//查找用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,//针对查找用户请求的回复
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,//添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,//针对添加好友请求的回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,//同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REJECT,//不同意添加好友
    ENUM_MSG_TYPE_REFRESH_FRIEND_REQUEST,//客户端刷新好友请求
    ENUM_MSG_TYPE_REFRESH_FRIEND_RESPOND,//针对客户端刷新好友的回复
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,//客户端删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,//针对客户端请求删除好友的回复
    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,//私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,//私聊回复
    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,//群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND,//群聊回复
    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,//创建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,//创建文件夹回复
    ENUM_MSG_TYPE_REFRESH_DIR_REQUEST,//查看文件夹请求
    ENUM_MSG_TYPE_REFRESH_DIR_RESPOND,//查看文件夹回复
    ENUM_MSG_TYPE_DELETE_DIR_REQUEST,//删除文件夹请求
    ENUM_MSG_TYPE_DELETE_DIR_RESPOND,//删除文件夹回复
    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,//重命名文件请求
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,//重命名文件回复
    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,//进入文件夹请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,//进入文件夹回复
    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,//上传请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,//上传回复
    ENUM_MSG_TYPE_DELETE_FILE_REQUEST,//删除常规文件请求
    ENUM_MSG_TYPE_DELETE_FILE_RESPOND,//删除常规文件回复
    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,//下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,//下载文件回复
    ENUM_MSG_TYPE_MOVE_FILE_REQUEST,//移动文件请求
    ENUM_MSG_TYPE_MOVE_FILE_RESPOND,//移动文件回复
    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,//共享文件请求
    ENUM_MSG_TYPE_SHARE_FILE_NOTE,//共享文件通知
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,//共享文件回复
    ENUM_MSG_TYPE_SHARE_FILE_AGREE,//同意共享文件
    ENUM_MSG_TYPE_LOGOUT_REQUEST,//注销请求
    ENUM_MSG_TYPE_LOGOUT_RESPOND,//注销回复
    ENUM_MSG_TYPE_MAX = 0x00ffffff //uint的最大值
};

struct FileInfo{
    char fileName[32]; //文件名字
    int fileType; //文件类型
};

struct PDU{
    uint ui_pdu_len; // 整个协议数据单元大小
    uint ui_msg_type; // 消息类型
    char usr_data[64]; // 文件信息
    uint ui_msg_len; // 实际消息长度
    int msg[]; // 实际消息内容
};

PDU *mkPDU(uint ui_msg_len);

#endif // PROTOCOL_H
