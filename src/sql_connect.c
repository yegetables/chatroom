#include "config.h"
#include PROJECT_SERVERHEAD

extern char database_name[20];
extern char database_user[20];
extern char database_passwd[100];
extern char database_ip[20];
extern int database_port;
extern zlog_category_t* ser;
MYSQL* sql_connect(void)
{
    zlog_debug(ser, "name:%s user:%s passwd:%s hosts:%s", database_name,
               database_user, database_passwd, database_ip);
    // CREATE DATABASE IF NOT EXISTS RUNOOB DEFAULT CHARSET utf8 COLLATE
    // utf8_general_ci;
    // 1. 如果数据库不存在则创建，存在则不创建。
    // 2. 创建RUNOOB数据库，并设定编码集为utf8
    MYSQL* conn = NULL;
    conn        = mysql_init(NULL);
    if (conn == NULL)
    {
        zlog_error(ser,
                   "Failed to init mysql "
                   "Error %u: %s",
                   mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    /// 连接数据库
    if (mysql_real_connect(conn, database_ip, database_user, database_passwd,
                           NULL, 0, NULL, 0) == NULL)
    {
        zlog_error(ser,
                   "Failed to connect "
                   "Error %u: %s",
                   mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    char q[BUFLEN];
    memset(q, 0, sizeof(q));
    sprintf(q,
            "CREATE DATABASE IF NOT EXISTS %s DEFAULT CHARSET utf8 COLLATE  "
            "utf8_general_ci;",
            database_name);
    if (mysql_query(conn, q))
    {
        zlog_warn(ser,
                  "failed to create "
                  "Error %u: %s",
                  mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    memset(q, 0, sizeof(q));
    sprintf(q, "use %s", database_name);
    if (mysql_query(conn, q))
    {
        zlog_warn(ser,
                  "failed to create "
                  "Error %u: %s",
                  mysql_errno(conn), mysql_error(conn));

        return NULL;
    }

    return conn;
    // mysql_close();
}