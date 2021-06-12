#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
extern int cfd;
extern int userid;
extern int applications;
extern int messages;
info* cli_send_recv(info* ms, int how)
{
    if (ms == NULL) return NULL;

    ms->how = how;

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
    else
    {
        char* t = showinfo(ms);
        zlog_debug(cli, "access failed:%s", t);
        free(t);
        if (ms)
        {
            free(ms);
            ms = NULL;
        }

        return ms;  // no close
    }
}

void update_notices(void)
{
    applications   = 0;
    messages       = 0;
    char p[BUFLEN] = {0};
    info* ms       = (info*)malloc(sizeof(info));
    {
        memset(p, 0, sizeof(p));
        sprintf(p,
                "select * from requests  where requests.to= \'%d\' and "
                "requests.how=\'%d\';",
                userid, ADD_FRIEND);

        {
            strcpy(ms->value, p);
            ms->type = sql;
            ms->from = userid;
            ms->to   = 0;
            ms->how  = MANY_RESULT;
        }
        ms = cli_send_recv(ms, MANY_RESULT);
        if (ms == NULL)
        {
            zlog_error(cli, "get applications failed ");
            return;
        }
    }

    applications = atoi(ms->value);

    {
        memset(p, 0, sizeof(p));
        sprintf(p,
                "select * from requests  where requests.to= \'%d\' and "
                "requests.how=\'%d\';",
                userid, MESSAGES);
        strcpy(ms->value, p);
        ms->type = sql;
        ms->from = userid;
        ms->to   = 0;
        ms->how  = MANY_RESULT;
        ms       = cli_send_recv(ms, MANY_RESULT);
        if (ms == NULL)
        {
            zlog_error(cli, "get messages failed ");
            return;
        }
    }

    messages = atoi(ms->value);

    if (ms) free(ms);
}