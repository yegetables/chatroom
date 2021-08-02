#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
extern char username[30];
extern int show_line;
void cli_show_friends(void)  // TODO(ajian): ready to perf
{
    char p[BUFLEN] = {0};
    int status;      //在线
    int shield;      // 屏蔽
    int number = 0;  //好友数量
    int online = 0;
    int j;
    printf("---------好友列表-----------\n");
    printf("yourname:%s  yourid:%d\n", username, userid);
    show_line += 2;
    info *ms = NULL;
    for (j = 0; j < 4; j++)
    {
        memset(p, 0, BUFLEN);
        {  //更改状态
            // 0->在线未屏蔽 10
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
            sprintf(p,
                    "select user.user_id,user.user_name from "
                    "relationship,user where "
                    "relationship.id_1= '%d' and "
                    "user.user_id=relationship.id_2 and "
                    "user.user_status= '%d' and relationship.if_shield= '%d' ;",
                    userid, status, shield);

            ms = cli_creatinfo(userid, 0, sql, FR_LIST, p);
            if (ms == NULL) break;
        }
        char *buf = ms->value;
        // show //%d\n%d %s\n%d %s\n
        int num;
        sscanf(buf, "%d", &num);
        buf = strchr(buf, '\n');

        for (int i = 0; i < num && ++buf != NULL; i++)  //本次个数
        {
            memset(p, 0, BUFLEN);  // name
            int id;
            sscanf(buf, "%d %s", &id, p);
            number++;
            if (j == 0) online++;
            buf = strchr(buf, '\n');
            printf("%2d-->%15s (%d)", number, p, id);
            if (j == 0)
            {
                printf("----(online)       \n");  //在线 未屏蔽
            }
            else if (j == 1)
            {
                printf("                   \n");  //离线 未屏蔽
            }
            else if (j == 2)
            {
                printf("----(online|shield)\n");  //在线 屏蔽
            }
            else
            {
                printf("----       (shield)\n");  //离线 屏蔽
            }
        }
    }

    if (j != 4)
    {
        zlog_error(cli, "can't find all j:%d", j);
        if (ms) free(ms);
        // printf("任意键退出\n");
        // getchar();
        // getchar();
        // show_line += 2;
        PAUSE;
        return;
    }

    printf("sum:%d friends,%d online\n", number,
           online);  //在线未屏蔽人数
    printf("------------------------\n");
    if (ms) free(ms);
    // printf("任意键退出\n");
    // getchar();
    // getchar();
    PAUSE;
    // show_line += 4 + number;
    show_line += 2 + number;
    return;
}

void cli_del_friend(void)
{
    int toid;
    printf("输入对方id\n");
    scanf("%9d", &toid);
    char p[BUFLEN] = {0};
    info *ms = NULL;
    sprintf(p,
            "delete from relationship where "
            "( relationship.id_1= '%d' and "
            "relationship.id_2= '%d' ) or "
            "( relationship.id_1= '%d' and "
            "relationship.id_2= '%d' ) ;",
            userid, toid, toid, userid);
    ms = cli_creatinfo(userid, 0, sql, HUP_NO, p);
    if (ms == NULL)
    {
        zlog_error(cli, "recv none");
    }
    else
    {
        zlog_debug(cli, "%d del %d success", userid, toid);
        if (ms) free(ms);
    }
    printf("删除%d成功\n", toid);
    PAUSE;
    show_line += 3;
}

void cli_shield_friend(int is)
{
    int toid;
    printf("输入对方id\n");
    scanf("%9d", &toid);
    char p[BUFLEN] = {0};
    info *ms = NULL;
    sprintf(p,
            "update relationship set if_shield= '%d' where "
            "relationship.id_1= '%d' and "
            "relationship.id_2= '%d' ;",
            is, userid, toid);
    ms = cli_creatinfo(userid, 0, sql, HUP_NO, p);
    if (ms == NULL)
    {
        zlog_error(cli, "recv none");
    }
    else
    {
        zlog_debug(cli, "%d sheild %d success", userid, toid);
    }
    printf("屏蔽状态更改%d成功\n", toid);
    // getchar();
    PAUSE;
    show_line += 3;

    if (ms) free(ms);
}

void cli_search_user(void)
{
reshow:;
    printf("1.输入id    2.输入namen 3.返回上一层\n");
    int t = 1;
    scanf("%1d", &t);
    char p[BUFLEN] = {0};
    memset(p, 0, BUFLEN);
    int iid = 10000;
    char na[30] = {0};
    if (t == 1)
    {
        scanf("%d", &iid);
        id_to_name(iid, p);
        printf("id:%d----name:%s\n", iid, p);
        PAUSE;
        show_line += 4;
    }
    else if (t == 2)
    {
        scanf("%s", na);
        printf("id:%d----name:%s\n", name_to_id(na), na);
        PAUSE;
        show_line += 4;
    }
    else if (t == 3)
    {
        return;
    }
    else
    {
        goto reshow;
    }
}

void cli_add_friend(void)
{
    int toid;
    printf("输入对方id\n");
    scanf("%9d", &toid);
    info *ms = NULL;
    char p[BUFLEN] = {0};
    char pp[BUFLEN / 2] = {0};
    sprintf(pp,
            "INSERT ignore INTO relationship (id_1,id_2,if_shield)"
            "VALUES (%d,%d,0),(%d,%d,0);",
            userid, toid, toid, userid);
    sprintf(p,
            "INSERT INTO requests "
            "(requests.from,requests.to,requests.type,requests.how,"
            "requests.value,requests.if_read) VALUES (%d,%d,%d,%d,\'%s\',0);",
            userid, toid, sql, ADD_FRIEND, pp);  //将来取走时候不回复
    ms = cli_creatinfo(userid, 0, sql, HUP_NO, p);
    if (ms == NULL)
    {
        zlog_error(cli, "recv none ");
        return;
    }
    printf("请求已发送,等待同意后在好友列表查看");
    PAUSE;
    show_line += 2;
    if (ms) free(ms);
}

void show_apply_friends(void)
{
    char p[BUFLEN] = {0};
    sprintf(p,
            "select requests.from,user.user_name from requests,user  where "
            "requests.to= %d and  requests.how=%d  and   "
            " user.user_id=requests.from",
            userid, ADD_FRIEND);
    info *ms = cli_creatinfo(userid, 0, sql, SHOW_APPLY, p);
    if (ms == NULL)
    {
        zlog_error(cli, "recv none");
        return;
    }
    char *buf = ms->value;
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
    show_line += num + 1;
    PAUSE;
    if (ms) free(ms);
    return;
}

void cli_agree_friend(int is)
{
    int id = 0;
    printf("输入对方id\n");
    scanf("%9d", &id);
    char p[BUFLEN] = {0};
    memset(p, 0, BUFLEN);
    info *ms = NULL;

    if (is == 1)  //同意
    {
        sprintf(p,
                "select requests.value  from requests  where "
                "requests.to= %d and  requests.how=%d and "
                "requests.from= %d",
                userid, ADD_FRIEND, id);
        ms = cli_creatinfo(userid, 0, sql, AGREE_APPLICATION, p);
        if (ms == NULL)
        {
            zlog_error(cli, "recv none");
            return;
        }
        if (ms) free(ms);
    }

    memset(p, 0, BUFLEN);
    //删除
    sprintf(p,
            "delete  from requests   where "
            "requests.to= %d and  requests.how=%d and "
            "requests.from= %d",
            userid, ADD_FRIEND, id);
    ms = cli_creatinfo(userid, 0, sql, HUP_NO, p);
    // base_GET_MANY_VALUE(ms,2)
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
    show_line += 3;
    PAUSE;
    if (ms) free(ms);
    return;
}
