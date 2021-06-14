#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
extern int userid;
extern char username[30];

void send_secret_message(int toid)
{ info* ms = (info*)malloc(sizeof(info));
    char p[BUFLEN / 2];
    printf("\nto:%d:", toid);
    memset(p, 0, sizeof(p));
    scanf("%s", p);
   
    ms->from = userid;
    ms->to   = 0;
    ms->type = sql;
    sprintf(ms->value,
            "INSERT  INTO requests "
            "(requests.from,requests.to,requests.type,requests.how,requests."
            "value,requests.if_read)"
            "VALUES (%d,%d,%d,%d,\'%s\',%d);",
            userid, toid, msg, MESSAGES, p, 0);
    ms = cli_send_recv(ms, HUP_NO);
    if (ms == NULL) return;
    if (ms) free(ms);
}

void recv_secret_message(int toid)
{
    info* ms = (info*)malloc(sizeof(info));
    ms->from = userid;
    ms->to   = 0;
    ms->type = sql;
    sprintf(
        ms->value,
        "select requests.add_time,requests.value from requests,relationship  "
        "where requests.to= %d and requests.how=%d and requests.from=%d and "
        "requests.from=relationship.id_2 and "
        "requests.to=relationship.id_1  and relationship.if_shield=0 and "
        "requests.if_read=0;",
        userid, MESSAGES, toid);  //未屏蔽,未读的消息

    ms = cli_send_recv(ms, SHOW_MESSAGES);
    if (ms == NULL) return;
    zlog_debug(cli, "recent_messages:%s\n", ms->value);
    printf("\nrecent_messages:%s", ms->value);
    memset(ms->value, 0, BUFLEN);
    sprintf(
        ms->value,
        "update requests,relationship set requests.if_read=1 "
        "where requests.to= %d and requests.how=%d and requests.from=%d and "
        "requests.from=relationship.id_2 and "
        "requests.to=relationship.id_1  and relationship.if_shield=0 and "
        "requests.if_read=0;",
        userid, MESSAGES, toid);  //设为已读
    ms->from = userid;
    ms->to   = 0;
    ms->type = sql;
    ms       = cli_send_recv(ms, HUP_NO);
    if (ms == NULL) return;
    if (ms) free(ms);
}