#include "config.h"
#include PROJECT_CLIENTHEAD
extern char username[30];
extern int userid;
extern zlog_category_t *cli;
extern int cfd;
int applications;
int messages;
int files;
extern int show_line;
extern char who_send_msg[BUFLEN];
extern char who_send_file[BUFLEN];
extern pthread_mutex_t update_mutex;
void information_bar(void)
{
    pthread_mutex_lock(&update_mutex);
    // update_notices(who_send_msg, who_send_file);
    //准备消息来源名单
    printf("你的id:%d-------%d:applications--------\n", userid, applications);
    printf("你的昵称:%s--------%d:messages from (%s)----------\n", username, messages,
           who_send_msg);
    printf("-----------%d:files from (%s)--------------\n", files, who_send_file);
    pthread_mutex_unlock(&update_mutex);
}

void show_main_menu(void)
{
    int c;
    while (1)
    {
        re_show();
        printf("1.好友列表\n");  //在线/昵称/id/屏蔽
        printf("2.好友管理\n");  // id
        printf("3.开始私聊\n");
        printf("4.群聊列表\n");
        printf("5.群聊管理\n");
        printf("6.开始群聊\n");
        printf("7.个人设置\n");
        // printf("8.找回密码\n");//登陆界面
        printf("8.好友设置\n");
        printf("0.退出登录\n");
        char *mm = readline("");
        // scanf("%2d", &c);
        c = atoi(mm);
        free(mm);
        show_line += 10;
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
                cli_show_groups();
                break;
            case 5:
                show_management_groups_menu();
                break;
            case 6:
                show_public_chat_menu();
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
    while (1)
    {
        re_show();
        printf("-----好友管理----\n");
        printf("1.添加好友\n");
        printf("2.删除好友\n");
        printf("3.屏蔽好友\n");
        printf("4.取消屏蔽\n");
        printf("5.查看好友申请\n");
        printf("6.搜索用户\n");
        printf("0.返回上一层\n");
        char *mm = readline("");
        // scanf("%2d", &c);
        c = atoi(mm);
        free(mm);
        show_line += 9;
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
    }
}

void show_management_groups_menu(void)
{
    int c;
    int toid;
    while (1)
    {
        re_show();
        printf("----群组管理----\n");
        printf("1.创建群组\n");
        printf("2.解散群族\n");
        printf("3.申请加入群组\n");
        printf("4.退出群组\n");
        printf("5.屏蔽群组\n");
        printf("6.取消屏蔽群组\n");
        printf("7.处理入群申请\n");
        printf("8.查看群成员\n");
        printf("9.设置群管理\n");
        printf("10.踢人\n");
        printf("0.返回上一层\n");
        char *mm = readline("");
        // scanf("%2d", &c);
        c = atoi(mm);
        free(mm);
        show_line += 11;
        switch (c)
        {
            case 0:  //返回上一层
                return;
            case 1:
                cli_create_groups(userid);
                break;
            case 2:
                cli_del_groups(userid);
                break;
            case 3:
                cli_add_group(userid, PEOPLE);
                break;
            case 4:
                cli_exit_group(userid);
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                show_group_applicationss_menu();

                break;
            case 8:
                cli_show_groups_members();
                break;
            case 9:

                printf("输入成员id\n");
                scanf("%9d", &toid);
                show_line += 2;
                cli_add_group(toid, MANER);
                break;
            case 10:
                printf("输入对方id\n");
                scanf("%9d", &toid);
                show_line += 2;
                cli_exit_group(toid);
                break;
            default:
                break;
        }
    }
}

void show_applicationss_menu(void)
{
    int c;
    re_show();
    printf("-----好友申请----\n");
    //列出申请列表
    show_apply_friends();
    printf("1.同意申请\n");
    printf("2.拒绝申请\n");
    printf("0.返回上一层\n");
    char *mm = readline("");
    // scanf("%2d", &c);
    c = atoi(mm);
    free(mm);
    show_line += 5;
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

void show_file_menu(int toid)
{
    int c;
    printf("-----文件待接受----\n");
    //列出申请列表
    show_apply_files(toid);
    printf("1.接受文件\n");
    printf("2.拒绝文件\n");
    printf("0.返回上一层\n");
    char *mm = readline("");
    // scanf("%2d", &c);
    c = atoi(mm);
    free(mm);
    show_line += 5;
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

void show_secret_chat_menu(void)
{
    int toid;
    {
        printf("输入对方id\n");
        printf("0.返回上一层\n");
        char *mm = readline("");
        // scanf("%2d", &c);
        toid = atoi(mm);
        free(mm);
        show_line += 3;
        if (toid == 0) return;
    }  //开始进入私聊界面
    char buf[1024] = {0};
    if (strlen(id_to_name(toid, buf)) == 0)
    {
        printf("error id, input again\n");
        show_line++;
        zlog_debug(cli, "error id, input again\n");
        return;
    }
    int c;
    while (1)
    {
        re_show();
        printf("-----私聊选项----\n");
        printf("1.发送消息\n");
        printf("2.发送文件\n");
        printf("3.查看消息记录\n");  //消息记录 (近一天?近10条?)
        printf("4.接收文件\n");
        printf("0.返回上一层\n");
        char *mm = readline("");
        // scanf("%2d", &c);
        c = atoi(mm);
        free(mm);
        show_line += 7;
        switch (c)
        {
            case 0:  // 返回上一层
                return;
            case 1:  // 私聊
                secret_message_menu(toid);
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
    }
}

void show_public_chat_menu(void)
{
    int toid;

    printf("输入群组id\n");
    printf("0.返回上一层\n");
    char *mm = readline("");
    // scanf("%2d", &c);
    toid = atoi(mm);
    free(mm);
    show_line += 3;
    if (toid == 0) return;
    int c;
    while (1)
    {
        re_show();
        printf("-----私聊选项----\n");
        printf("1.发送消息\n");
        printf("2.查看消息记录\n");
        printf("0.返回上一层\n");
        char *mm = readline("");
        // scanf("%2d", &c);
        c = atoi(mm);
        free(mm);
        show_line += 5;
        switch (c)
        {
            case 0:  // 返回上一层
                return;
            case 1:
                public_message_menu(toid);
                break;
            case 2:
                show_public_message(toid);
                break;
            default:
                break;
        }
    }
}
void show_group_applicationss_menu(void)
{
    int c;
    int groupid;
    printf("输入群组id\n");
    char *mm = readline("");
    // scanf("%2d", &c);
    groupid = atoi(mm);
    free(mm);
    show_line += 2;
    while (1)
    {
        re_show();
        printf("-----群组申请----\n");
        //列出申请列表
        cli_show_group_applications(groupid);
        printf("1.同意申请\n");
        printf("2.拒绝申请\n");
        printf("0.返回上一层\n");
        char *mm = readline("");
        // scanf("%2d", &c);
        c = atoi(mm);
        free(mm);
        show_line += 5;
        switch (c)
        {
            case 0:  //返回上一层
                return;
            case 1:  //同意好友申请
                cli_apply_application(1, groupid);
                break;
            case 2:  //删除好友申请
                cli_apply_application(2, groupid);
                break;
            default:
                break;
        }
        return;
    }
}
