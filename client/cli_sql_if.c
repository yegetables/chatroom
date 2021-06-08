#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
extern int cfd;

bool cli_sql_if(char* p, int how)
{
    if (p == NULL) return false;
    char* tt = NULL;
    // 构造info结构
    info mms;
    {
        memset(&mms, 0, sizeof(mms));
        strcpy(mms.value, p);
        mms.type = sql;
        mms.from = -1;
        mms.to   = 0;
        mms.how  = how;
    }

    // 发送查询sql
    if (!send_info(cfd, &mms)) return false;  // no close

    // 接受并验证结果
    if (recv_info(cfd, &mms))
    {
        tt = showinfo(&mms);
        zlog_debug(cli, tt);
        free(tt);
        if (mms.how == IF_DONE || mms.how == IF_HAS)  //简单真假判断
        {
            if (mms.value[0] == '1')
                return true;
            else
                return false;
        }
        return false;
    }
    else
    {
        zlog_debug(cli, "access failed");  // no close
        return false;
    }
    return false;
}

bool cli_register(char* name, char* passwd)
{
    if (name == NULL || passwd == NULL) return false;
    // sql语句
    char p[BUFLEN] = {0};

    // 构造查询语句
    sprintf(p, "INSERT INTO user (user_name,user_passwd ) VALUES ( '%s','%s');",
            name, passwd);

    return cli_sql_if(p, IF_DONE);
}

bool cli_accesspasswd(char* name, char* passwd)
{
    if (name == NULL) return false;
    // sql语句
    char p[BUFLEN] = {0};

    // 构造查询语句
    if (passwd == NULL)
        sprintf(p, "select * from user where user_name ='%s';", name);
    else
        sprintf(p,
                "select * from user where user_name ='%s' and "
                "user_passwd='%s';",
                name, passwd);

    return cli_sql_if(p, IF_HAS);
}

// TODO: check online
bool cli_accessonline(char* name) { return false; }