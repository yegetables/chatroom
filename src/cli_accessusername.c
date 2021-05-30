#include "config.h"
#include PROJECT_CLIENTHEAD

extern zlog_category_t* cli;
extern int cfd;
bool cli_accessusername(char* name)
{
    char p[BUFLEN];
    memset(p, 0, sizeof(p));
    sprintf(p, "select * from user where username ='%s';", name);
    info ms;
    ms.vaule = p;
    ms.type  = msg;
    ms.from  = -1;
    ms.to    = 0;
    if (sizeof(info) != Writen(cfd, &ms, sizeof(info)))
    {
        zlog_warn(cli, "write username failed");
    }
}