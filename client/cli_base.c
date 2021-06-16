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

    if (how != HUP_NO)
    {
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
    else
    {
        return ms;
    }
}

void update_notices(int* who_send)
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
                "select distinct requests.from from requests,relationship  "
                "where requests.to= "
                "\'%d\' and "
                "requests.how=\'%d\' and requests.from=relationship.id_2 and "
                "requests.to=relationship.id_1  and relationship.if_shield=0 "
                "and requests.if_read=0 ;",
                userid, MESSAGES);  //未屏蔽的消息
        strcpy(ms->value, p);
        ms->type = sql;
        ms->from = userid;
        ms->to   = 0;
        ms       = cli_send_recv(ms, GET_MESSAGE_FROM);
        if (ms == NULL)
        {
            zlog_error(cli, "get messages failed ");
            return;
        }
    }
    messages = atoi(ms->value);
    char* b  = strchr(ms->value, '\n');
    if (b == NULL)
    {
        zlog_error(cli, "value:%s", ms->value);
        exit(-1);
    }

    b++;
    for (int i = 0; i < messages && b != NULL; i++, b++)
    {
        sscanf(b, "%d", &who_send[i]);
        b = strchr(b, '\n');
    }
    if (ms) free(ms);
}
