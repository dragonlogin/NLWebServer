//
// Created by nl on 2020/9/9.
//

#include "sql_connection_pool.h"
using namespace std;

ConnectionPool::ConnectionPool() : m_cur_conn(0), m_free_conn(0){

}

ConnectionPool *ConnectionPool::GetInstance()
{
    static ConnectionPool connPool;
    return &connPool;
}

void ConnectionPool::init(string url, string user, string password, string DBname, int port, int max_conn, int close_log)
{
    m_url = url;
    m_port = port;
    m_user = user;
    m_password = password;
    m_database_name = DBname;
    m_close_log = close_log;

    for (int i = 0; i < max_conn; i++)
    {
        MYSQL *con = NULL;
        con = mysql_init(con);

        if (con == NULL)
        {
            LOG_ERROR("MySQL Error");
            exit(1);
        }
        con = mysql_real_connect(con, url.c_str(), user.c_str(), password.c_str(), DBname.c_str(), port, NULL, 0);

        if (con == NULL)
        {
            LOG_ERROR("MySQL Error");
            exit(1);
        }
        conn_list.push_back(con);
        ++m_free_conn;
    }

    reserve = Sem(m_free_conn);

    m_max_conn = m_free_conn;
}

//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL* ConnectionPool::GetConnection()
{
    MYSQL *con = NULL;
    if (0 == conn_list.size())
        return NULL;

    reserve.wait();

    {
        MutexLockGuard lock(m_mutex);
        con = conn_list.front();
        conn_list.pop_front();

        --m_free_conn;
        ++m_cur_conn;
    }
    return con;
}

//释放当前使用的连接
bool ConnectionPool::ReleaseConnection(MYSQL *con)
{
    if (NULL == con)
        return false;

    {
        MutexLockGuard lock(m_mutex);
        conn_list.push_back(con);
        ++m_free_conn;
        --m_cur_conn;
    }

    reserve.post();
    return true;
}

//销毁数据库连接池
void ConnectionPool::DestroyPool()
{

    {
        MutexLockGuard lock(m_mutex);
        if (conn_list.size() > 0)
        {

            for (auto it = conn_list.begin(); it != conn_list.end(); ++it)
            {
                mysql_close(*it);
            }
            m_cur_conn = 0;
            m_free_conn = 0;
            conn_list.clear();
        }
    }

}

//当前空闲的连接数
int ConnectionPool::GetFreeConn()
{
    return this->m_free_conn;
}

ConnectionPool::~ConnectionPool()
{
    DestroyPool();
}

ConnectionRAII::ConnectionRAII(MYSQL **SQL, ConnectionPool *connPool){
    *SQL = connPool->GetConnection();

    conRAII = *SQL;
    poolRAII = connPool;
}

ConnectionRAII::~ConnectionRAII(){
    poolRAII->ReleaseConnection(conRAII);
}