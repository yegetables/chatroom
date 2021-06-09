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
    // 记录错误次数
    int errornumber = 0;

    // zlog_debug(ser, "name:%s user:%s passwd:%s hosts:%s", database_name,
    // database_user, database_passwd, database_ip);
    // CREATE DATABASE IF NOT EXISTS RUNOOB DEFAULT CHARSET utf8 COLLATE
    // utf8_general_ci;
    // 1. 如果数据库不存在则创建，存在则不创建。
    // 2. 创建RUNOOB数据库，并设定编码集为utf8
    MYSQL* conn = NULL;
    errornumber = 0;
reinit:
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        errornumber++;
        zlog_error(ser,
                   "Failed to init mysql "
                   "Error %u: %s",
                   mysql_errno(conn), mysql_error(conn));
        if (errornumber > 3)
            return NULL;
        else
        {
            sleep(rand() % 5);
            goto reinit;
        }
    }
    else
    {
        ;  // zlog_debug(ser, "init mysql success");
    }

    /// 连接数据库
    errornumber = 0;
reconnect:
    if (mysql_real_connect(conn, database_ip, database_user, database_passwd,
                           NULL, 0, NULL, 0) == NULL)
    {
        errornumber++;
        zlog_error(ser,
                   "Failed to connect "
                   "Error %u: %s",
                   mysql_errno(conn), mysql_error(conn));
        if (errornumber > 3)
            return NULL;
        else
        {
            sleep(rand() % 5);
            goto reconnect;
        }
    }
    else
    {
        zlog_info(ser, "connect datebase success");
    }

    char q[BUFLEN];
    memset(q, 0, sizeof(q));
    sprintf(q,
            "CREATE DATABASE IF NOT EXISTS %s DEFAULT CHARSET utf8 COLLATE  "
            "utf8_general_ci;",
            database_name);
    if (mysql_query(conn, q))
    {
        zlog_error(ser,
                   "failed to create "
                   "Error %u: %s",
                   mysql_errno(conn), mysql_error(conn));
        return NULL;
    }
    else
    {
        ;  // zlog_debug(ser, "datebase creat success");
    }

    memset(q, 0, sizeof(q));
    sprintf(q, "use %s", database_name);
    if (mysql_query(conn, q))
    {
        zlog_error(ser,
                   "failed to create "
                   "Error %u: %s",
                   mysql_errno(conn), mysql_error(conn));

        return NULL;
    }
    else
    {
        ;  // zlog_debug(ser, "use datebase %s success", database_name);
    }

    return conn;
    // mysql_close();
}

bool sql_init_table(MYSQL* sql_l)
{
    char q[BUFLEN];

    /// @note user table
    {
        memset(q, 0, sizeof(q));
        sprintf(q,
                "CREATE TABLE IF NOT EXISTS `user` ( "
                "`user_id` INT UNSIGNED  AUTO_INCREMENT PRIMARY KEY, "
                "`user_name` VARCHAR(25) NOT NULL, "
                "`user_passwd` VARCHAR(30) NOT NULL,"
                "`user_last_login_time` DATE "
                ")default charset utf8;");
        //    AUTO_INCREMENT定义列为自增的属性，一般用于主键，数值会自动加1。
        //    PRIMARY KEY关键字用于定义列为主键。
        //    您可以使用多列来定义主键，列间以逗号分隔。

        if (mysql_query(sql_l, q))
        {
            zlog_error(ser,
                       "failed to create "
                       "Error %u: %s",
                       mysql_errno(sql_l), mysql_error(sql_l));
            return false;
        }
        else
        {
            ;  // zlog_debug(ser, "table user created success");
        }
        memset(q, 0, sizeof(q));
        sprintf(q, "ALTER TABLE user AUTO_INCREMENT=10000;");
        if (mysql_query(sql_l, q))
        {
            zlog_error(ser,
                       "failed to create "
                       "Error %u: %s",
                       mysql_errno(sql_l), mysql_error(sql_l));
            return false;
        }
        else
        {
            ;  // zlog_debug(ser, "table user id auto_increment=10000");
        }
    }
    /// @note relationship
    {
        memset(q, 0, sizeof(q));
        sprintf(q,
                "CREATE TABLE IF NOT EXISTS `relationship` ( "
                "`id_1` INT UNSIGNED , "
                "`id_2` INT UNSIGNED "
                ");");
        //    AUTO_INCREMENT定义列为自增的属性，一般用于主键，数值会自动加1。
        //    PRIMARY KEY关键字用于定义列为主键。
        //    您可以使用多列来定义主键，列间以逗号分隔。

        if (mysql_query(sql_l, q))
        {
            zlog_error(ser,
                       "failed to create "
                       "Error %u: %s",
                       mysql_errno(sql_l), mysql_error(sql_l));
            return false;
        }
        else
        {
            ;  // zlog_debug(ser, "table relationship created success");
        }
    }

    return true;
}