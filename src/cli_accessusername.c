#include "config.h"
#include PROJECT_CLIENTHEAD

extern zlog_category_t* cli;
extern int cfd;
bool cli_accessusername(char* name)
{
    char p[BUFLEN];
    memset(p, 0, sizeof(p));
    sprintf(p, "select * from user where username ='%s';", name);
    message ms;
    ms.vaule = p;
    ms.type  = msg;
    ms.from  = -1;
    ms.to    = 0;
    write(cfd, &ms, sizeof(message));
    
}