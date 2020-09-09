//
// Created by nl on 2020/9/9.
//

#ifndef NLWEBSERVER_SQL_CONNECTION_POOL_H
#define NLWEBSERVER_SQL_CONNECTION_POOL_H
#include <mysql/mysql.h>
#include "../lock/locker.h"
#include "../log/log.h"
using namespace std;

class ConnectionPool {
public:
    MYSQL *GetConnection();
    bool ReleaseConnection(MYSQL *conn); //释放连接
    int GetFreeConn();					 //获取连接
    void DestroyPool();					 //销毁所有连接

    //单例模式
    static ConnectionPool *GetInstance();

    void init(string url, string User, \
            string PassWord, string DataBaseName, \
            int Port, int MaxConn, int close_log);
private:
    ConnectionPool();
    ~ConnectionPool();
    int m_max_conn;  //最大连接数
    int m_cur_conn;  //当前已使用的连接数
    int m_free_conn; //当前空闲的连接数
    mutable Locker m_mutex;
    list<MYSQL*> conn_list; //连接池
    Sem reserve;

private:
    string m_url;			 //主机地址
    string m_port;		 //数据库端口号
    string m_user;		 //登陆数据库用户名
    string m_password;	 //登陆数据库密码
    string m_database_name; //使用数据库名
    int m_close_log;	//日志开关
};


class ConnectionRAII{

public:
    ConnectionRAII(MYSQL **con, ConnectionPool *connPool);
    ~ConnectionRAII();

private:
    MYSQL *conRAII;
    ConnectionPool *poolRAII;
};
#endif //NLWEBSERVER_SQL_CONNECTION_POOL_H
