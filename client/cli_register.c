#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int cfd;

bool cli_register(char *name, char *passwd)
{
    if (name == NULL || passwd == NULL) return false;
    // sql语句
    char p[BUFLEN] = {0};

    // 构造查询语句
    sprintf(p, "INSERT INTO user (user_name,user_passwd ) VALUES ( '%s','%s');",
            name, passwd);

    return cli_sql_if(p);
}