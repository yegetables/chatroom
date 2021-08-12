#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
extern char username[30];
extern int show_line;
// extern pthread_attr_t detach_attr;
extern pthread_mutex_t rs_mutex;
int send_secret_message(int toid)
{
    info *ms = NULL;
    // char p[BUFLEN / 2] = {0};
    char pp[BUFLEN] = {0};
    // printf("\nto:%d:", toid);
    // scanf("%s", p);
    fflush(stdout);
    sprintf(pp, "\n            to:%d:", toid);
    char *p = readline(pp);
    show_line += 2;
    if (strncmp(p, "#return#", 8) == 0)
    {
        free(p);
        return 0;  //返回上一层
    }
    memset(pp, 0, BUFLEN);
    sprintf(pp,
            "INSERT  INTO requests "
            "(requests.from,requests.to,requests.type,requests.how,requests."
            "value,requests.if_read)"
            "VALUES (%d,%d,%d,%d,\'%s\',%d);",
            userid, toid, msg, MESSAGES, p, 0);
    free(p);
    ms = cli_creatinfo(userid, 0, sql, HUP_NO, pp);
    if (ms == NULL) return 0;
    if (ms) free(ms);
    return 1;  //继续聊天
}

void *recv_secret_message(void *ttoid)
{
    info *ms = NULL;
    int toid = *(int *)ttoid;
    char p[BUFLEN] = {0};
    while (1)
    {
        pthread_testcancel();
        sprintf(p,
                "select requests.from,requests.add_time,requests.value from requests,relationship  "
                "where requests.to= %d and requests.how=%d and requests.from=%d and "
                "requests.from=relationship.id_2 and "
                "requests.to=relationship.id_1  and relationship.if_shield=0 and "
                "requests.if_read=0;",
                userid, MESSAGES, toid);  //未屏蔽,未读的消息
        ms = cli_creatinfo(userid, 0, sql, SHOW_MESSAGES, p);
        if (ms == NULL)
        {
            // pthread_testcancel();
            return NULL;
        }
        int number = atoi(ms->value);
        if (number != 0)
        {
            char *buf = strchr(ms->value, '\n');
            buf++;
            printf("\nfrom %s", buf);
            // zlog_error(cli, "why 乱码?\n%s", buf);
            printf("\n            to:%d:", toid);
            fflush(stdout);
            show_line += number + 1 + 2;
        }
        memset(p, 0, BUFLEN);
        sprintf(p,
                "update requests,relationship set requests.if_read=1 "
                "where requests.to= %d and requests.how=%d and requests.from=%d and "
                "requests.from=relationship.id_2 and "
                "requests.to=relationship.id_1  and relationship.if_shield=0 and "
                "requests.if_read=0;",
                userid, MESSAGES, toid);  //设为已读
        ms = cli_creatinfo(userid, 0, sql, HUP_NO, p);
        if (ms == NULL)
        {
            // pthread_testcancel();
            return NULL;
        }
        if (ms) free(ms);
        sleep(1);
    }
}

void show_secret_message(int toid)
{
    info *ms = NULL;
    char p[BUFLEN] = {0};
    sprintf(
        p,
        "select requests.from,requests.add_time,requests.value from requests,relationship  "
        "where ((requests.to= %d and requests.from=%d)or(requests.to= %d and requests.from=%d)) "
        "and requests.how=%d  and "
        "requests.from=relationship.id_2 and "
        "requests.to=relationship.id_1  ;",
        userid, toid, toid, userid, MESSAGES);  //未屏蔽,未读的消息
    ms = cli_creatinfo(userid, 0, sql, SHOW_MESSAGES, p);
    if (ms == NULL) return;
    int number = atoi(ms->value);
    printf("have %d messages:\n", number);
    show_line++;
    if (number != 0)
    {
        char *buf = strchr(ms->value, '\n');
        buf++;
        char relbuf[BUFLEN] = {0};
        char *q = NULL;
        int newid;
        for (int i = 0; i < number && buf != NULL; i++)
        {
            memset(relbuf, 0, BUFLEN);
            q = strchr(buf, '\n');
            strncpy(relbuf, buf, (int)(q - buf + 1));
            sscanf(relbuf, "%d", &newid);
            if (newid == userid) printf("          ");
            printf("from %s", relbuf);
            buf = strchr(buf, '\n');
            buf++;
        }
    }
    show_line += number;
    PAUSE;
    if (ms) free(ms);
}

void secret_message_menu(int toid)
{
    int returnnumber = 1;
    pthread_t recv_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // pthread_setcancletype(&recv_thread, PTHREAD_CANCEL_DEFFERED);

    printf("随时输入(#return#)返回上一层\n");
    show_line++;
    pthread_create(&recv_thread, &attr, recv_secret_message, &toid);
    while (returnnumber)
    {
        // recv_secret_message(toid);
        returnnumber = send_secret_message(toid);
    }
    pthread_cancel(recv_thread);
    usleep(1);
    pthread_join(recv_thread, NULL);
    pthread_attr_destroy(&attr);
    return;
}

void recv_public_message(int toid)
{
    info *ms = NULL;
    char p[BUFLEN] = {0};
    sprintf(p,
            "select requests.add_time,requests.from,requests.value from requests,relationship  "
            "where requests.to= %d and requests.how=%d  and "
            "requests.from=relationship.id_2 and "
            "requests.to=relationship.id_1  and relationship.if_shield=0 and "
            "requests.if_read=0;",
            toid, MESSAGES);  //未屏蔽,未读的消息

    ms = cli_creatinfo(userid, 0, sql, SHOW_GROUP_MESSAGES, p);
    if (ms == NULL) return;
    int number = atoi(ms->value);
    if (number != 0)
    {
        char *buf = strchr(ms->value, '\n');
        buf++;
        printf("to %d:%s", toid, buf);  // BUG:showline
        show_line += number;
    }
    // TODO(ajian): 群聊消息设置已读
    // memset(p, 0, BUFLEN);
    // sprintf(p,
    // 	"update requests,relationship set requests.if_read=1 "
    // 	// "where requests.to= %d and requests.how=%d and
    // 	// requests.from=%d and " "requests.from=relationship.id_2 and "
    // 	// "requests.to=relationship.id_1  and relationship.if_shield=0
    // 	// and " "requests.if_read=0;",
    // 	"where requests.to= %d and requests.how=%d  and "
    // 	"requests.from=relationship.id_2 and "
    // 	"requests.to=relationship.id_1  and relationship.if_shield=0 and
    // " 	"requests.if_read=0;", 	toid, MESSAGES); //设为已读 ms =
    // cli_creatinfo(userid, 0, sql, HUP_NO, p);
    if (ms == NULL) return;
    if (ms) free(ms);
}

void show_public_message(int toid)
{
    info *ms = NULL;
    char p[BUFLEN] = {0};
    sprintf(p,
            "select requests.add_time,requests.from,requests.value from requests,relationship  "
            "where requests.to= %d and requests.how=%d  and "
            "requests.from=relationship.id_2 and "
            "requests.to=relationship.id_1  ;",
            toid, MESSAGES);  //未屏蔽,未读的消息
    ms = cli_creatinfo(userid, 0, sql, SHOW_GROUP_MESSAGES, p);
    if (ms == NULL) return;
    int number = atoi(ms->value);
    printf("have %d messages:\n", number);
    show_line++;
    if (number != 0)
    {
        char *buf = strchr(ms->value, '\n');
        buf++;
        printf("to %d:%s", toid, buf);  // BUG:showline
        show_line += number;
    }
    PAUSE;
    if (ms) free(ms);
}

void public_message_menu(int toid)
{
    int returnnumber = 1;
    printf("随时输入(#return#)返回上一层\n");
    show_line++;
    while (returnnumber)
    {
        recv_public_message(toid);                 //
        returnnumber = send_secret_message(toid);  // ok
    }
    return;
}