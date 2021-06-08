#include "config.h"
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
extern MYSQL *sql_l;

bool sql_do(events *ev)
{
    info *ms  = &(ev->js);
    char *buf = ms->value;
    MYSQL_RES *result;
    // 执行
    int returnnumber = mysql_query(sql_l, buf);
    if (ms->how == IF_DONE)
    {
        if (returnnumber)
        {
            memset(buf, 0, BUFLEN);
            zlog_error(ser, "执行时出现异常: %s", mysql_error(sql_l));
            buf[0] = '0';
        }
        else
        {
            memset(buf, 0, BUFLEN);
            buf[0] = '1';
        }
        return true;
    }

    if (!returnnumber)
    {
        // mysql_store_result保存查询到的数据到result
        // mysql_num_rows返回结果集中的行数
        // MYSQL_ROW *row = mysql_fetch_row();
        result = mysql_store_result(sql_l);
        if (ms->how == IF_HAS)  //如果询问存在
        {
            memset(buf, 0, BUFLEN);
            if (mysql_num_rows(result))
                buf[0] = '1';
            else
                buf[0] = '0';
            return true;  //处理完毕
        }

        if (ms->how == MANY_RESULT)
        {
            memset(buf, 0, BUFLEN);
            itoa(mysql_num_rows(result), buf, 10);
            return true;  //处理完毕
        }
        zlog_error(ser, "don't know which how");
        return false;
    }
    zlog_error(ser, "执行时出现异常: %s", mysql_error(sql_l));
    return false;
}
// bool ser_accessname() { return false; }
