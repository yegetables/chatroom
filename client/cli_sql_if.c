#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
extern int cfd;
extern int userid;

info* cli_sql_if(char* p, int how)
{
    if (p == NULL) return NULL;

    // 构造info结构
    info* ms = (info*)malloc(sizeof(info));
    {
        strcpy(ms->value, p);
        ms->type = sql;
        ms->from = userid;  //
        ms->to   = 0;
        ms->how  = how;
    }

    // 发送查询sql
    if (!send_info(cfd, ms))
    {
        if (ms)
        {
            free(ms);
            ms = NULL;
        }
        return ms;  // no close
    }
    // 接受并展示info
    if (recv_info(cfd, ms))
    {
        char* tt = NULL;
        tt       = showinfo(ms);
        zlog_debug(cli, tt);
        free(tt);
        return ms;
    }
    if (ms)
    {
        free(ms);
        ms = NULL;
    }
    zlog_debug(cli, "access failed");  // no close
    return ms;
}

bool cli_register(char* name, char* passwd, char* email)  // IF_DONE
{
    if (name == NULL || passwd == NULL || email == NULL) return false;
    // sql语句
    char p[BUFLEN] = {0};

    // 构造insert语句
    sprintf(p,
            "INSERT INTO user (user_name,user_passwd, user_email,user_status) "
            "VALUES ( "
            "\'%s\',\'%s\',\'%s\',0);",
            name, passwd, email);
    info* ms = cli_sql_if(p, IF_DONE);
    int s    = atoi(ms->value);
    if (ms) free(ms);
    if (s)
        return true;
    else
        return false;
}

bool cli_accesspasswd(char* name, char* passwd)  // IF_HAS
{
    if (name == NULL) return false;
    // sql语句
    char p[BUFLEN] = {0};

    // 构造select语句
    if (passwd == NULL)
        sprintf(p, "select * from user where user_name =\'%s\';", name);
    else
        sprintf(p,
                "select * from user where user_name =\'%s\' and "
                "user_passwd=\'%s\';",
                name, passwd);

    info* ms = cli_sql_if(p, IF_HAS);
    int s    = atoi(ms->value);
    if (ms) free(ms);
    if (s)
        return true;
    else
        return false;
}

bool cli_accessonline(char* name)  // WHAT_FIRST_VALUE
{
    if (name == NULL) return false;
    // sql语句
    char p[BUFLEN] = {0};
    // 构造select语句
    sprintf(p, "SELECT user_status  FROM user where user_name =\'%s\';", name);

    info* ms = cli_sql_if(p, WHAT_FIRST_VALUE);
    int s    = atoi(ms->value);
    if (ms) free(ms);
    if (s)
        return true;
    else
        return false;
}

int cli_setonline(char* name)  // SET_ONLINE
{
    if (name == NULL) return -1;
    // sql语句
    char p[BUFLEN] = {0};
    // 构造update语句
    sprintf(p, "update user set  user_status= \'1\'  where user_name= \'%s\' ;",
            name);

    info* ms = cli_sql_if(p, SET_ONLINE);
    int s    = atoi(ms->value);
    if (ms) free(ms);
    if (s > 0)
        return s;
    else
        return -1;
}


