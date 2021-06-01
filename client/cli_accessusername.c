#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
extern int cfd;
bool cli_accessusername(char* name)
{
    char p[BUFLEN];
    memset(p, 0, sizeof(p));
    sprintf(p, "select * from user where user_name ='%s';", name);
    info ms;
    memset(&ms, 0, sizeof(ms));
    strcpy(ms.vaule, p);
    ms.type = sql;
    ms.from = -1;
    ms.to   = 0;

    //非阻塞写

    if (sizeof(info) != send(cfd, &ms, sizeof(info), 0))
    {
        zlog_warn(cli, "write username failed");
        return false;
        //@todo: fix errno
    }
    else
    {
        //验证结果
        memset(&ms, 0, sizeof(info));
        memset(p, 0, sizeof(p));
        bool bo = 0;
        if (recv(cfd, &bo, sizeof(bool), 0) != sizeof(bool))
        {
            if (errno == EINTR || errno == SIGPIPE)
            {
                return false;
            }  //@todo: fix errno
        }
        else if (bo)
            return true;
        else
            return false;
    }
}