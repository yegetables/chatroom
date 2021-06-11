#include "config.h"
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
extern MYSQL *sql_l;
extern char fd_name[MAXCLIENT][30];
extern int fd_id[MAXCLIENT];
bool sql_do(events *ev)
{
    info *ms = &(ev->js);
    switch (ms->how)
    {
        case SET_ONLINE:
            if (!base_sql_query(ms)) return false;
            if (!event_set_online(ev)) return false;
            break;
        case IF_DONE:
            if (!base_sql_query(ms)) return false;
            if (!case_IF_DONE(ms)) return false;
            break;
        case IF_HAS:
            if (!base_sql_query(ms)) return false;
            if (!case_IF_HAS(ms)) return false;
            break;
        case MANY_RESULT:
            if (!base_sql_query(ms)) return false;
            if (!case_MANY_RESULT(ms)) return false;
            break;
        case WHAT_FIRST_VALUE:
            if (!base_sql_query(ms)) return false;
            if (!case_WHAT_FIRST_VALUE(ms)) return false;
            break;
        default:
            zlog_warn(ser, "don't know which how");
            return false;
    }
    return true;

    {  // mysql_store_result保存查询到的数据到result
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
}

bool base_sql_query(info *ms)
{
    char *buf = ms->value;
    int returnnumber;
    returnnumber = -1;
    returnnumber = mysql_query(sql_l, buf);
    if (returnnumber)  //出错
    {
        zlog_error(ser, "执行时出现异常: %s", mysql_error(sql_l));
        return false;
    }
    else
        return true;
}

//询问第一个值是多少
bool case_WHAT_FIRST_VALUE(info *ms)
{
    char *buf         = ms->value;
    MYSQL_RES *result = NULL;
    MYSQL_ROW rowline;
    result = mysql_store_result(sql_l);
    if (result == NULL)  //出错
    {
        zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
        mysql_free_result(result);
        return false;
    }
    memset(buf, 0, BUFLEN);
    rowline = mysql_fetch_row(result);  //第一行
    strcpy(buf, rowline[0]);            //第一列
    ms->to   = ms->from;
    ms->from = 0;
    mysql_free_result(result);
    return true;
}

//询问有多少行数据
bool case_MANY_RESULT(info *ms)
{
    char *buf         = ms->value;
    MYSQL_RES *result = NULL;
    result            = mysql_store_result(sql_l);
    if (result == NULL)  //出错
    {
        zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
        mysql_free_result(result);
        return false;
    }
    memset(buf, 0, BUFLEN);
    itoa(mysql_num_rows(result), buf, 10);
    ms->to   = ms->from;
    ms->from = 0;
    mysql_free_result(result);
    return true;
}

//询问存在
bool case_IF_HAS(info *ms)
{
    char *buf         = ms->value;
    MYSQL_RES *result = NULL;
    result            = mysql_store_result(sql_l);
    if (result == NULL)  //出错
    {
        zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
        mysql_free_result(result);
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
    return true;
}

//执行成功
bool case_IF_DONE(info *ms)
{
    char *buf = ms->value;
    memset(buf, 0, BUFLEN);
    strcpy(buf, "1");
    ms->to   = ms->from;
    ms->from = 0;
    return true;
}

//设置在线
bool event_set_online(events *ev)
{  // get name(设置status=1)
    info *ms  = &(ev->js);
    char *buf = ms->value;
    {  //获取name更改为查询id
        char na[30] = {0};
        char temp[30];
        sscanf(buf, "%s %s %s %s %s %s %s %s ;", temp, temp, temp, temp, temp,
               temp, temp, na);
        na[strlen(na) - 1] = 0;  // zlog_debug(ser, "na = %s", &na[1]);
        memset(buf, 0, BUFLEN);
        // 更改查询语句
        sprintf(buf, "select user_id  from user where user_name=\'%s\';",
                &na[1]);
    }
    if (!base_sql_query(ms)) return false;
    //将id返回
    if (!case_WHAT_FIRST_VALUE(ms)) return false;
    {  //设置在线状态
        int id        = atoi(buf);
        fd_id[ev->fd] = id;
        id_to_name(id, fd_name[ev->fd]);  // fd->name
        zlog_info(ser, "**online**   fd:%d id:%d name:%s", ev->fd,
                  fd_id[ev->fd], fd_name[ev->fd]);
    }
    return true;
}
