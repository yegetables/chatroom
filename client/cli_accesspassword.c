#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
extern int cfd;
bool cli_accesspasswd(char* name, char* passwd)
{
    // sql语句
    char p[BUFLEN];

    // 构造查询语句
    {
        memset(p, 0, sizeof(p));
        if (passwd == NULL)

        {
            sprintf(p, "select * from user where user_name ='%s';", name);
        }
        else
        {
            sprintf(p,
                    "select * from user where user_name ='%s' and "
                    "user_passwd='%s';",
                    name, passwd);
        }
    }

    return cli_sql_if(p);
}