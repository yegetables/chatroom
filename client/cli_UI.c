#include "config.h"
#include PROJECT_CLIENTHEAD
extern char username[30];
extern int userid;
extern zlog_category_t* cli;
extern int cfd;
int applications;
int messages;
void information_bar(void)
{
    int who_send[MAXCLIENT];
    memset(who_send, 0, sizeof(who_send));
    char buf[MAXCLIENT * 30];
    memset(buf, 0, sizeof(buf));
    update_notices(who_send);
    for (int i = 0; i < messages; i++)
        sprintf(&buf[strlen(buf)], "%d ", who_send[i]);
    printf("----------%d:applications--------\n", applications);
    printf("------------%d:messages from (%s)----------\n", messages, buf);
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
reshow:
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
reshow:
    printf("-----私聊选项----\n");
    printf("1.发送消息\n");
    printf("2.发送文件\n");
    printf("3.查看消息记录\n");  //消息记录 (近一天?近10条?)
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
    char path[MAX_PATH] = {0};
    scanf("%s", path);
    //检测文件权限

    if (!access(path, F_OK) && !access(path, R_OK))
    {
        struct stat statbuf;
        memset(&statbuf, 0, sizeof(struct stat));
        if (lstat(path, &statbuf))
        {
            zlog_error(ser, "fstat error %s", strerror(errno));
            return false;
        };
        statbuf.printf("waiting transmission\n");
        //发送文件通知
        info* ms = (info*)malloc(sizeof(info));
        ms->from = userid;
        ms->to   = toid;
        ms->type = msg;
        sprintf(ms->value, "%s", path);
        ms = cli_send_recv(ms, SEND_FILE_REDY);
        if (ms == NULL)
        {
            zlog_error(cli, "send file requuest rejust");
            return;
        }

        //真正发送文件
        if (!send_file(cfd, path))
        {
            zlog_error("send file error: %s", path);
            return;
        }
    }
    else
    {
        zlog_error("path:%s can't read ");
        return;
    }
}
