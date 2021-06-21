#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
extern int userid;
extern char username[30];

void cli_show_friends(void)
{
    char p[BUFLEN] = {0};
    int status;      //在线
    int shield;      // 屏蔽
    int number = 0;  //好友数量
    int online = 0;
    int j;
    info* ms = (info*)malloc(sizeof(info));
    for (j = 0; j < 4; j++)
    {
        memset(p, 0, sizeof(p));
        //更改状态
        {  // 0->在线未屏蔽 10
            // 1->离线未屏蔽 00
            // 2->在线屏蔽 11
            // 3->离线屏蔽 01
            if (j == 0)
            {
                status = 1;
                shield = 0;
            }
            else if (j == 1)
            {
                status = 0;
                shield = 0;
            }
            else if (j == 2)
            {
                status = 1;
                shield = 1;
            }
            else if (j == 3)
            {
                status = 0;
                shield = 1;
            }

            {
                sprintf(
                    p,
                    "select user.user_id,user.user_name from "
                    "relationship,user where "
                    "relationship.id_1= '%d' and "
                    "user.user_id=relationship.id_2 and "
                    "user.user_status= '%d' and relationship.if_shield= '%d' ;",
                    userid, status, shield);
            }
        }

        {
            strcpy(ms->value, p);
            ms->type = sql;
            ms->from = userid;
            ms->to   = 0;
        }
        ms = cli_send_recv(ms, FR_LIST);  // base_GET_MANY_VALUE(ms, 2)
        if (ms == NULL) break;
        char* buf = ms->value;
        if (j == 0)
        {
            printf("---------好友列表-----------\n");
            printf("yourname:%s  yourid:%d\n", username, userid);
        }
        // show //%d\n%d %s\n%d %s\n
        int num;
        sscanf(buf, "%d", &num);
        buf = strchr(buf, '\n');
        memset(p, 0, BUFLEN);                           // name
        for (int i = 0; i < num && ++buf != NULL; i++)  //本次个数
        {
            int id;
            sscanf(buf, "%d %s", &id, p);
            number++;
            if (j == 0) online++;
            buf = strchr(buf, '\n');
            printf("%2d-->%15s (%d)", number, p, id);
            if (j == 0)
                printf("----(online)       \n");  //在线 未屏蔽
            else if (j == 1)
                printf("                   \n");  //离线 未屏蔽
            else if (j == 2)
                printf("----(online|shield)\n");  //在线 屏蔽
            else
                printf("----       (shield)\n");  //离线 屏蔽
        }
    }
    if (j != 4)
    {
        zlog_error(cli, "can't find all j:%d", j);
        if (ms) free(ms);
        return;
    }
    printf("sum:%d friends,%d online\n", number, online);  //在线未屏蔽人数
    printf("------------------------\n");
    if (ms) free(ms);
}

void cli_del_friend(void)
{
    int toid;
    printf("输入对方id\n");
    scanf("%d", &toid);
    char p[BUFLEN] = {0};
    memset(p, 0, BUFLEN);
    sprintf(p,
            "delete from relationship where "
            "( relationship.id_1= '%d' and "
            "relationship.id_2= '%d' ) or "
            "( relationship.id_1= '%d' and "
            "relationship.id_2= '%d' ) ;",
            userid, toid, toid, userid);
    info* ms = (info*)malloc(sizeof(info));
    {
        strcpy(ms->value, p);
        ms->how  = IF_DONE;
        ms->type = sql;
        ms->from = userid;
        ms->to   = 0;
    }
    ms = cli_send_recv(ms, HUP_NO);
    if (ms == NULL)
    {
        zlog_error(cli, "recv none");
        return;
    }
    // char* buf = ms->value;
    // if (buf[0] == '1')
    // {
    printf("删除%d成功\n", toid);
    zlog_debug(cli, "%d del %d success", userid, toid);
    // }
    // else
    // {
    //     zlog_error(cli, "%d del %d  error", userid, toid);
    //     printf("删除%d失败\n", toid);
    // }

    if (ms) free(ms);
}

void cli_shield_friend(int is)
{
    int toid;
    printf("输入对方id\n");
    scanf("%d", &toid);
    char p[BUFLEN] = {0};
    memset(p, 0, BUFLEN);
    sprintf(p,
            "update relationship set if_shield= '%d' where "
            "relationship.id_1= '%d' and "
            "relationship.id_2= '%d' ;",
            is, userid, toid);
    info* ms = (info*)malloc(sizeof(info));
    {
        strcpy(ms->value, p);
        ms->how  = IF_DONE;
        ms->type = sql;
        ms->from = userid;
        ms->to   = 0;
    }
    ms = cli_send_recv(ms, HUP_NO);
    if (ms == NULL)
    {
        zlog_error(cli, "recv none");
        return;
    }
    // char* buf = ms->value;
    // if (buf[0] == '1')
    // {
    printf("屏蔽%d成功\n", toid);
    zlog_debug(cli, "%d sheild %d success", userid, toid);
    // }
    // else
    // {
    //     zlog_error(cli, "%d sheild %d  error", userid, toid);
    //     printf("屏蔽%d失败\n", toid);
    // }
    if (ms) free(ms);
}

void cli_search_user(void)
{
reshow:;
    printf("1.输入id    2.输入namen 3.返回上一层\n");
    int t = 1;
    scanf("%d", &t);
    char p[BUFLEN] = {0};
    memset(p, 0, BUFLEN);
    int iid     = 10000;
    char na[30] = {0};
    if (t == 1)
    {
        scanf("%d", &iid);
        id_to_name(iid, p);
        printf("id:%d----name:%s", iid, p);
    }
    else if (t == 2)
    {
        scanf("%s", na);
        printf("id:%d----name:%s\n", name_to_id(na), na);
    }
    else if (t == 3)
        return;
    else
        goto reshow;
}

void cli_add_friend(void)
{
    int toid;
    printf("输入对方id\n");
    scanf("%d", &toid);
    info* ms = (info*)malloc(sizeof(info));

    char p[BUFLEN]      = {0};
    char pp[BUFLEN / 2] = {0};
    {
        memset(p, 0, sizeof(p));
        memset(pp, 0, sizeof(pp));
        sprintf(pp,
                "INSERT ignore INTO relationship (id_1,id_2,if_shield)"
                "VALUES (%d,%d,0),(%d,%d,0);",
                userid, toid, toid, userid);
        // BUG:ignore 无效
        sprintf(p,
                "INSERT INTO requests "
                "(requests.from,requests.to,requests.type,requests.how,"
                "requests.value,requests.if_read) VALUES (%d,%d,%d,%d,\'%s\',0);",
                userid, toid, sql, ADD_FRIEND, pp);  //将来取走时候不回复
    }

    {
        strcpy(ms->value, p);
        ms->type = sql;
        ms->from = userid;
        ms->to   = 0;
        ms->how  = IF_DONE;
    }
    ms = cli_send_recv(ms, HUP_NO);
    if (ms == NULL)
    {
        zlog_error(cli, "recv none ");
        return;
    }
    // if (ms->value[0] == '0')
    //     zlog_error(cli, "add error ");
    // else
    printf("请求已发送,等待同意后在好友列表查看\n");

    if (ms) free(ms);
}

void show_apply_friends(void)
{
    char p[BUFLEN] = {0};
    memset(p, 0, BUFLEN);
    sprintf(p,
            "select requests.from,user.user_name from requests,user  where "
            "requests.to= %d and  requests.how=%d  and   "
            " user.user_id=requests.from",
            userid, ADD_FRIEND);
    info* ms = (info*)malloc(sizeof(info));
    {
        strcpy(ms->value, p);
        ms->how  = SHOW_APPLY;
        ms->type = sql;
        ms->from = userid;
        ms->to   = 0;
    }
    ms = cli_send_recv(ms, SHOW_APPLY);  // base_GET_MANY_VALUE(ms, 2)
    if (ms == NULL)
    {
        zlog_error(cli, "recv none");
        return;
    }
    char* buf = ms->value;
    // printf("%s\n", buf);
    int num = 0;
    sscanf(buf, "%d", &num);
    buf = strchr(buf, '\n');                         // name
    for (int i = 1; i <= num && ++buf != NULL; i++)  //本次个数
    {
        int id;
        sscanf(buf, "%d %s", &id, p);
        buf = strchr(buf, '\n');
        printf("%2d-->%15s (%d)\n", i, p, id);
    }
    printf("----------sum:%d--------\n", num);
    if (ms) free(ms);
    return;
}

void cli_agree_friend(int is)
{
    int id = 0;
    printf("输入对方id\n");
    scanf("%d", &id);
    char p[BUFLEN] = {0};
    memset(p, 0, BUFLEN);
    info* ms = (info*)malloc(sizeof(info));
    if (is == 1)  //同意
    {
        sprintf(p,
                "select requests.value  from requests  where "
                "requests.to= %d and  requests.how=%d and "
                "requests.from= %d",
                userid, ADD_FRIEND, id);

        {
            strcpy(ms->value, p);
            ms->type = sql;
            ms->from = userid;
            ms->to   = 0;
        }
        ms =
            cli_send_recv(ms, AGREE_APPLICATION);  // base_GET_MANY_VALUE(ms, 2)
        if (ms == NULL)
        {
            zlog_error(cli, "recv none");
            return;
        }
    }

    memset(p, 0, BUFLEN);
    //删除
    sprintf(p,
            "delete  from requests   where "
            "requests.to= %d and  requests.how=%d and "
            "requests.from= %d",
            userid, ADD_FRIEND, id);
    {
        strcpy(ms->value, p);
        ms->type = sql;
        ms->from = userid;
        ms->to   = 0;
    }
    ms = cli_send_recv(ms, HUP_NO);  // base_GET_MANY_VALUE(ms, 2)
    if (ms == NULL)
    {
        zlog_error(cli, "recv none");
        return;
    }
    if (is == 1)
    {
        zlog_debug(cli, "agree and del from %d to %d ", id, userid);
        printf("已同意%d的申请\n", id);
    }
    else
    {
        zlog_debug(cli, " del from %d to %d ", id, userid);
        printf("已拒绝%d的申请\n", id);
    }
    if (ms) free(ms);
    return;
}

