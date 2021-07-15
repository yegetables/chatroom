#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
extern char username[30];
extern int show_line;

void cli_show_groups(void)
{
    char p[BUFLEN] = {0};
    int number = 0;
    sprintf(p,
            "select group_id,group_name from "
            " groups,relationship where "
            " relationship.id_1=%d and "
            " relationship.id_2=group_id  ",
            userid);
    info *ms = cli_creatinfo(userid, 0, sql, GET_MANY_VALUE, p);
    if (ms == NULL)
    {
        zlog_error(cli, "recv error");
        return;
    }
    printf("---------群组列表-----------\n");
    printf("yourname:%s  yourid:%d\n", username, userid);
    show_line += 2;
    char *buf = ms->value;
    // show //%d\n%d %s\n%d %s\n
    int num;
    sscanf(buf, "%d", &num);
    buf = strchr(buf, '\n');

    for (int i = 0; i < num && ++buf != NULL; i++) //本次个数
    {
        memset(p, 0, BUFLEN); // name
        int id;
        sscanf(buf, "%d %s", &id, p);
        number++;
        buf = strchr(buf, '\n');
        printf("%2d-->%15s (%d)\n", number, p, id);
    }
    printf("sum:%d groups\n", number); //在线未屏蔽人数
    printf("------------------------\n");
    show_line += 2 + number;
    PAUSE;
    if (ms)
        free(ms);
    return;
}

int cli_create_groups(int uid)
{
    char groupname[20] = {0};
    int groupid = -1;
    printf("输入群聊名称\n");
    scanf("%s", groupname);
    char p[BUFLEN] = {0};
    sprintf(p, "insert into groups (group_name,master_id)values(\'%s\',%d);", groupname, userid);
    info *ms = cli_creatinfo(userid, 0, sql, CREATE_GROUP, p);
    if (ms == NULL)
    {
        zlog_error(cli, "recv none ");
        return groupid;
    }
    groupid = atoi(ms->value);
    printf("群组id为%d\n", groupid);
    show_line += 3;
    PAUSE;
    if (ms)
        free(ms);
    return groupid;
}

bool cli_del_groups(int userid)
{
    // char groupname[20] = { 0 };
    int groupid = -1;
    printf("输入群聊id\n");
    scanf("%d", &groupid);
    char p[BUFLEN] = {0};

    sprintf(p,
            "select * from groups where  group_id=%d  and "
            "master_id=%d;",
            groupid, userid);
    info *ms = cli_creatinfo(userid, groupid, sql, DEL_GROUP, p);
    if (ms == NULL)
    {
        zlog_error(cli, "recv none ");
        return false;
    }
    if (atoi(ms->value) != 1)
    {
        printf("未找到群主为%d,群号为%d的群或者您不是群主\n", userid, groupid);
    }
    else
    {
        printf("群组id为%d的群已解散\n", groupid);
    }
    show_line += 3;
    if (ms)
        free(ms);
    PAUSE;
    return true;
}

void cli_show_groups_members(void)
{
    int groupid = -1;
    printf("输入已加入的群组id查看群成员\n");
    scanf("%d", &groupid);
    show_line += 2;

    char p[BUFLEN] = {0};
    int number = 0;
    sprintf(p,
            "select user.user_id,user.user_name from "
            " groups,relationship,user where "
            // " relationship.id_1=%d and "
            // " relationship.id_2=groups.group_id and  "
            " relationship.id_1=user.user_id and "
            " relationship.id_2=%d ",
            groupid);
    info *ms = cli_creatinfo(userid, groupid, sql, GR_LIST, p);
    if (ms == NULL)
    {
        zlog_error(cli, "recv error");
        return;
    }
    if (atoi(ms->value) == -1)
    {
        printf("您没有权限");
        PAUSE;
        if (ms)
            free(ms);
        return;
    }

    printf("---------群组成员-----------\n");
    // printf("yourname:%s  yourid:%d\n", username, userid);
    show_line += 1;
    char *buf = ms->value;
    // show //%d\n%d %s\n%d %s\n
    int num;
    sscanf(buf, "%d", &num);
    buf = strchr(buf, '\n');

    for (int i = 0; i < num && ++buf != NULL; i++)
    {
        memset(p, 0, BUFLEN);
        int id;
        sscanf(buf, "%d %s", &id, p);
        number++;
        buf = strchr(buf, '\n');
        printf("%2d-->%15s (%d)\n", number, p, id);
    }
    printf("sum:%d members\n", number);
    printf("------------------------\n");
    show_line += 2 + number;
    PAUSE;
    if (ms)
        free(ms);
    return;
}

void cli_add_group(int toid, int Authority)
{
    int groupid;
    printf("输入群组id\n");
    scanf("%d", &groupid);
    show_line += 2;
    char p[BUFLEN] = {0};
    sprintf(p, "%d %d", Authority, toid);
    info *ms;
    if (toid == userid)
    { //申请加入n->p
        ms = cli_creatinfo(userid, groupid, sql, ADD_GROUP_APPLY, p);
    }
    else
    { //设置别人升权降权
        ms = cli_creatinfo(userid, groupid, sql, set_POWER_GROUP, p);
    }
    if (atoi(ms->value) != 1)
    {
        printf("请求出错  ");
    }
    else
    {
        printf("请求已发送");
    }
    PAUSE;
}
void cli_exit_group(int toid)
{
    int groupid;
    printf("输入群组id\n");
    scanf("%d", &groupid);
    show_line += 2;
    char p[BUFLEN] = {0};
    sprintf(p, "%d", toid);
    info *ms = cli_creatinfo(userid, groupid, sql, EXIT_GROUP, p);
    // u->g  toid,exit

    if (atoi(ms->value) != -1)
    {
        printf("已退出群%d", groupid);
    }
    else
    {
        printf("%d退群出错", toid);
    }
    PAUSE;
}

void cli_apply_application(int is, int groupid)
{
    int id = 0;
    printf("输入对方id\n");
    scanf("%d", &id);
    char p[BUFLEN] = {0};
    memset(p, 0, BUFLEN);
    info *ms = NULL;

    if (is == 1) //同意
    {
        sprintf(p, "%d %d", id, groupid);
        ms = cli_creatinfo(userid, 0, sql, ADD_GROUP, p);
        if (ms == NULL)
        {
            zlog_error(cli, "recv none");
            return;
        }
        if (ms)
            free(ms);
    }

    memset(p, 0, BUFLEN);
    //删除
    sprintf(p,
            "delete  from requests   where "
            "requests.to=%d  and  requests.how=%d and "
            "requests.from= %d",
            groupid, ADD_GROUP_APPLY, id);
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
    if (ms)
        free(ms);
    return;
}

void cli_show_group_applications(int groupid)
{
    char p[BUFLEN] = {0};
    sprintf(p,
            " select user.user_id,user.user_name from "
            " user,requests,groups where  requests.to=%d "
            " and requests.how=%d and requests.from=user.user_id ;",
            groupid, ADD_GROUP_APPLY);
    info *ms = cli_creatinfo(userid, groupid, sql, SHOW_GROUP_APPLY, p);
    if (ms == NULL)
        return;
    char *buf = ms->value;
    int num = 0;
    sscanf(buf, "%d", &num);
    // printf("num:%d\n", num);
    if (num == -1)
    {
        printf("无权限\n");
        if (ms)
            free(ms);
        return;
    }
    buf = strchr(buf, '\n');                        // name
    for (int i = 1; i <= num && ++buf != NULL; i++) //本次个数
    {
        int id;
        sscanf(buf, "%d %s", &id, p);
        buf = strchr(buf, '\n');
        printf("%2d-->%15s (%d)\n", i, p, id);
    }
    printf("----------sum:%d--------\n", num);
    show_line += num + 1;
    if (ms)
        free(ms);
    return;
}
