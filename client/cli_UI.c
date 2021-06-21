#include "config.h"
#include PROJECT_CLIENTHEAD
extern char username[30];
extern int userid;
extern zlog_category_t* cli;
extern int cfd;
int applications;
int messages;
int files;
void information_bar(void)
{
    char* who_send_msg  = (char*)calloc(BUFLEN, sizeof(char));
    char* who_send_file = (char*)calloc(BUFLEN, sizeof(char));
    update_notices(who_send_msg, who_send_file);
    //准备消息来源名单
    printf("----------%d:applications--------\n", applications);
    printf("------------%d:messages from (%s)----------\n", messages,
           who_send_msg);
    printf("-----------%d:files from (%s)--------------\n", files,
           who_send_file);
    free(who_send_msg);
    free(who_send_file);
}
void show_main_menu(void)
{
    int c;
    while (1)
    {
        // gudingxinxi
        information_bar();

        printf("1.好友列表\n");  //在线/昵称/id/屏蔽
        printf("2.好友管理\n");  // id
        printf("3.私聊\n");
        printf("4.群聊列表\n");

        printf("3.进入聊天界面\n");

        printf("4.个人设置\n");
        // printf("8.找回密码\n");//登陆界面
        printf("5.好友设置\n");
        printf("0.退出登录\n");
        printf("-------------------\n");

        scanf("%d", &c);
        switch (c)
        {
            case 1:
                cli_show_friends();  //好友列表
                break;
            case 2:
                show_management_friends_menu();  //好友管理
                break;
            case 3:
                show_secret_chat_menu();  //私聊
                break;
            case 4:
                break;
            case 5:
                break;
            case 0:
                zlog_fini();
                close(cfd);
                exit(1);
            default:
                break;
        }
    }
}

void show_management_friends_menu(void)
{
    int c;
reshow:;
    printf("-----好友管理----\n");
    printf("1.添加好友\n");
    printf("2.删除好友\n");
    printf("3.屏蔽好友\n");
    printf("4.取消屏蔽\n");
    printf("5.查看好友申请\n");
    printf("6.搜索用户\n");
    printf("0.返回上一层\n");
    scanf("%d", &c);
    switch (c)
    {
        case 0:  //返回上一层
            return;
        case 1:  //添加好友
            cli_add_friend();
            break;
        case 2:  //删除好友
            cli_del_friend();
            break;
        case 3:  //屏蔽好友
            cli_shield_friend(1);
            break;
        case 4:  //取消屏蔽
            cli_shield_friend(0);
            break;
        case 5:  //查看好友申请
            show_applicationss_menu();
            break;
        case 6:  // 搜索用户
            cli_search_user();
            break;
        default:
            break;
    }
    goto reshow;
}

void show_applicationss_menu(void)
{
    int c;
    printf("-----好友申请----\n");
    //列出申请列表
    show_apply_friends();
    printf("1.同意申请\n");
    printf("2.拒绝申请\n");
    printf("0.返回上一层\n");
    scanf("%d", &c);
    switch (c)
    {
        case 0:  //返回上一层
            return;
        case 1:  //同意好友申请
            cli_agree_friend(1);
            break;
        case 2:  //删除好友申请
            cli_agree_friend(2);
            break;
        default:
            break;
    }
    return;
}

void cli_recv_file(int toid, int is)
{
    char fname[PATH_MAX / 2] = {0};  //文件模糊名
    info* ms                 = NULL;
    char sname[BUFLEN];    //本地保存文件路径
    char relname[BUFLEN/2];  //服务器真实文件名
    char* b;
resdd:;
    memset(fname, 0, PATH_MAX);
    printf("输入文件名\n");
    scanf("%s", fname);

    // char realpath[PATH_MAX] = {0};
    // char p[BUFLEN] = {0};
    // memset(p, 0, BUFLEN);
    ms = (info*)malloc(sizeof(info));
    {  //选择接受的文件
        sprintf(ms->value,
                "select requests.how,requests.value  from requests  where "
                "requests.to= %d and requests.from=%d and requests.type=%d"
                " and requests.value like \'%%%s\'",
                userid, toid, file, fname);  //模糊搜索
        // zlog_warn(cli, "p=%s", p);
        // strcpy(ms->value, p);
        ms->type = sql;
        ms->from = userid;
        ms->to   = 0;
    }
    ms = cli_send_recv(ms, AGREE_RECV_FILE);  // base_GET_MANY_VALUE(ms, 2)
    if (ms == NULL)
    {
        zlog_error(cli, "recv none");
        return;
    }
    if (1 != atoi(ms->value))
    {
        if (ms) free(ms);
        printf("结果太多或不存在,重新搜索\n");
        goto resdd;
    }
    //唯一结果
    long f_size;
    b = strchr(ms->value, '\n');
    b++;
    sscanf(b, "%ld %s", &f_size, relname);

    if (is == 1)  //同意
    {
        memset(sname, 0, PATH_MAX);
        printf("输入保存到文件的绝对路径\n");
        scanf("%s", sname);
        // char* f = dirname(fname);             //目录
        // sprintf(realpath, "%s/%s", f, name);  //拼接目录/文件

        zlog_info(cli, "save %s to path %s", relname, sname);

        //打开文件准备存储
        if (!recv_file(cfd, sname, f_size))
        {
            zlog_error(cli, "recv file error: %s", sname);
            if (ms) free(ms);
            return;
        }
    }
    // fname 客户接受路径

    //设置已读
    {
        sprintf(ms->value,
                "update requests,relationship set requests.if_read=1 "
                "where "
                "requests.to= %d   and requests.value =\'%s\' and  "
                "requests.type=%d and "
                "requests.from= %d",
                userid, relname, file, toid);  //设为已读

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
        zlog_debug(cli, "recv file %s to %s del from %d to %d ", relname, sname,
                   toid, userid);
        printf("已接受文件%s to %s\n", relname, sname);
    }
    else
    {
        zlog_debug(cli, " del file from %d to %d ", toid, userid);
        printf("已拒绝文件%s\n", fname);
    }
    if (ms) free(ms);
    return;
}

void show_file_menu(int toid)
{
    int c;
    printf("-----文件待接受----\n");
    //列出申请列表
    show_apply_files(toid);
    printf("1.接受文件\n");
    printf("2.拒绝文件\n");
    printf("0.返回上一层\n");
    scanf("%d", &c);
    switch (c)
    {
        case 0:  //返回上一层
            return;
        case 1:  //同意
            cli_recv_file(toid, 1);
            break;
        case 2:  //删除
            cli_recv_file(toid, 2);
            break;
        default:
            break;
    }
    return;
}

void show_apply_files(int toid)
{
    char p[BUFLEN] = {0};
    memset(p, 0, BUFLEN);

    info* ms = (info*)malloc(sizeof(info));
    {
        sprintf(ms->value,
                "select requests.value   from requests,relationship  "
                "where requests.to= "
                "\'%d\' and "
                "requests.type=\'%d\' and requests.from=\'%d\' and "
                "requests.from=relationship.id_2 and "
                "requests.to=relationship.id_1  and relationship.if_shield=0 "
                "and requests.if_read=0 ;",
                userid, file, toid);  //未屏蔽的文件

        ms->type = sql;
        ms->from = userid;
        ms->to   = 0;
    }
    ms = cli_send_recv(ms, GET_MESSAGE_FROM);  // base_GET_MANY_VALUE(ms, 1)
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
        memset(p, 0, sizeof(p));
        sscanf(buf, "%s", p);
        buf = strchr(buf, '\n');
        printf("%2d-->%15s \n", i, p);
        // printf("%2d-->%15s \n", i, basename(p));
    }
    printf("----------sum:%d--------\n", num);
    if (ms) free(ms);
    return;
}

void show_secret_chat_menu(void)
{
    int toid;
    {
        printf("输入对方id\n");
        printf("0.返回上一层\n");
        scanf("%d", &toid);
        if (toid == 0) return;
    }  //开始进入私聊界面
    char buf[1024] = {0};
    if (strlen(id_to_name(toid, buf)) == 0)
    {
        printf("error id, input again\n");
        zlog_debug(cli, "error id, input again\n");
        return;
    }
    int c;
reshow:;
    printf("-----私聊选项----\n");
    printf("1.发送消息\n");
    printf("2.发送文件\n");
    printf("3.查看消息记录\n");  //消息记录 (近一天?近10条?)
    printf("4.接收文件\n");
    printf("0.返回上一层\n");
    scanf("%d", &c);
    switch (c)
    {
        case 0:  // 返回上一层
            return;
        case 1:  // 私聊
            message_menu(toid);
            break;
        case 2:  // 发文件
            send_file_menu(toid);
            break;
        case 3:  // 历史记录(不显示文件历史)
            show_secret_message(toid);
            break;
        case 4:  //收文件
            show_file_menu(toid);
            break;
        default:
            break;
    }
    goto reshow;
}

void message_menu(int toid)
{
    int returnnumber = 1;
    printf("随时输入(#return#)返回上一层\n");
    while (returnnumber)
    {
        recv_secret_message(toid);
        returnnumber = send_secret_message(toid);
    }
    return;
}

void send_file_menu(int toid)
{
    // cat /proc/sys/kernel/random/uuid 随机uuid
    printf("输入文件路径\n");
    char path[PATH_MAX] = {0};
    scanf("%s", path);
    //检测文件名长度
    if (strlen(path) > 50)
    {
        printf("file name too long \n");
        zlog_error(cli, "file name too long ");
        return;
    }

    // 内部检测权限
    long int f_size = get_file_size(path);
    if (f_size < 0)
    {
        zlog_error(cli, "get size error");
        return;
    }

    char* filename = basename(path);
    printf("waiting transmission\n");

    //发送文件通知
    info* ms = (info*)malloc(sizeof(info));
    {
        ms->from = userid;
        ms->to   = 0;
        ms->type = sql;
        sprintf(ms->value, "%s %ld %d", filename, f_size, toid);
        zlog_debug(cli, "sendfile ready value: %s", ms->value);
    }
    //发送文件通知
    ms = cli_send_recv(ms, SEND_FILE_REDY);
    if (ms == NULL || atoi(ms->value) == 0)
    {
        zlog_error(cli, "path:%s can't ready file ", path);
        printf("服务器未准备好接收文件\n");
        return;
    }

    //真正发送文件
    if (!send_file(cfd, path, f_size))
    {
        zlog_error(cli, "path:%s can't read ", path);
        return;
    }

    // 接受并展示info
    if (recv_info(cfd, ms))
    {
        if (!atoi(ms->value))
            zlog_info(cli, "server can't  recv all");
        else
            zlog_info(cli, " recv file message success");
    }
    else
    {
        zlog_info(cli, "can;t recv file message");
    }

    if (ms)
    {
        free(ms);
        ms = NULL;
    }
    zlog_debug(cli, "recv errror");
    return;  // no close
}
