
#include "config.h"
#include PROJECT_SERVERHEAD
extern zlog_category_t* ser;
bool sql_init_table(MYSQL* sql)
{
    char q[BUFLEN];
    memset(q, 0, sizeof(q));
    sprintf(q,
            "CREATE TABLE IF NOT EXISTS `user` ( "
            "`user_id` INT UNSIGNED  AUTO_INCREMENT PRIMARY KEY, "
            "`user_name` VARCHAR(25) NOT NULL, "
            "`user_last_login_time` DATE "
            ")default charset utf8;");
    //    AUTO_INCREMENT定义列为自增的属性，一般用于主键，数值会自动加1。
    //    PRIMARY KEY关键字用于定义列为主键。
    //    您可以使用多列来定义主键，列间以逗号分隔。

    if (mysql_query(sql, q))
    {
        zlog_warn(ser,
                  "failed to create "
                  "Error %u: %s",
                  mysql_errno(sql), mysql_error(sql));
        return false;
    }
    memset(q, 0, sizeof(q));
    sprintf(q, "ALTER TABLE user AUTO_INCREMENT=10000;");
    if (mysql_query(sql, q))
    {
        zlog_warn(ser,
                  "failed to create "
                  "Error %u: %s",
                  mysql_errno(sql), mysql_error(sql));
        return false;
    }

    return true;
}