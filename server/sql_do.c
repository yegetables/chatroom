#include "config.h"
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
extern MYSQL *sql_l;
extern char fd_name[MAXCLIENT][30];
extern int fd_id[MAXCLIENT];
bool sql_do(events *ev)
{
    info *ms          = &(ev->js);
    char *buf         = ms->value;
    MYSQL_RES *result = NULL;
    MYSQL_ROW rowline;
    int returnnumber;
redo:
    returnnumber = -1;
    returnnumber = mysql_query(sql_l, buf);
    if (returnnumber)  //出错
    {
        zlog_error(ser, "执行时出现异常: %s", mysql_error(sql_l));
        return false;
    }

    switch (ms->how)
    {
        case SET_ONLINE:
        {
            char na[30] = {0};
            char temp[30];
            sscanf(buf, "%s %s %s %s %s %s %s %s ;", temp, temp, temp, temp,
                   temp, temp, temp, na);
            na[strlen(na) - 1] = 0;
            // zlog_debug(ser, "na = %s", &na[1]);
            memset(buf, 0, BUFLEN);
            sprintf(buf, "select user_id  from user where user_name=\'%s\';",
                    &na[1]);
            ms->how = WHAT_FIRST_VALUE;

            goto redo;
        }
        break;
        case IF_DONE:
        {
            memset(buf, 0, BUFLEN);
            strcpy(buf, "1");
            ms->to   = ms->from;
            ms->from = 0;
        }
        break;
        case IF_HAS:  //如果询问存在
        {
            result = mysql_store_result(sql_l);
            if (result == NULL)  //出错
            {
                zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
                return false;
            }
            memset(buf, 0, BUFLEN);
            if (mysql_num_rows(result))
                strcpy(buf, "1");
            else
                strcpy(buf, "0");
            ms->to   = ms->from;
            ms->from = 0;
            mysql_free_result(result);
        }
        break;
        case MANY_RESULT:  //询问有多少行数据
        {
            result = mysql_store_result(sql_l);
            if (result == NULL)  //出错
            {
                zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
                return false;
            }
            memset(buf, 0, BUFLEN);
            itoa(mysql_num_rows(result), buf, 10);
            ms->to   = ms->from;
            ms->from = 0;
            mysql_free_result(result);
        }
        break;
        case WHAT_FIRST_VALUE:  //询问第一个值是多少
        {
            result = mysql_store_result(sql_l);
            if (result == NULL)  //出错
            {
                zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
                return false;
            }
            memset(buf, 0, BUFLEN);
            rowline = mysql_fetch_row(result);  //第一行
            strcpy(buf, rowline[0]);            //第一列
            ms->to   = ms->from;
            ms->from = 0;
            mysql_free_result(result);
        }
        break;
        default:
            zlog_warn(ser, "don't know which how");
            return false;
    }
    return true;

    // mysql_store_result保存查询到的数据到result
    // mysql_num_rows返回结果集中的行数
    // MYSQL_ROW *row = mysql_fetch_row();
    // while (row = mysql_fetch_row(res))
    // {
    //     for (t = 0; t < mysql_num_fields(res); t++)

    //     {
    //         printf(
    //             "%s"
    //             " ",
    //             row[t]);
    //     }

    //     printf("%s", row[1]);
    //     //表示查询的第几个字段的！也还可以遍历全部字段信息
    // }
}
