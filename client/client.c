#include "config.h"
#include PROJECT_CLIENTHEAD

zlog_category_t *cli = NULL;
int cfd;
int main(int argc, char **argv)
{
    /// @brief 开日志
    char cmd[100] = {0};
#ifdef DEBUG
    sprintf(cmd, "rm ");
    strcat(cmd, PROJECT_LOGPATH);
    strcat(cmd, "client*.log");
    system(cmd);
#endif
    cli = my_zlog_init("client");
    zlog_info(cli, "--------start--------");

    /// @brief 解析命令行
    char *options = "h";
    int opt;
    while ((opt = getopt(argc, argv, options)) != -1)
    {
        /**
         * @brief getopt
         * @note optarg —— 指向当前选项参数(如果有) 的指针。
         * @note optind —— 再次调用 getopt()时的下一个 argv指针的索引。
         * @note optopt —— 最后一个未知选项。
         * @note opterr ­——
         * 如果不希望getopt()打印出错信息，则只要将全域变量opterr设为0即可。
         */

        if (opt == 'h') clienthelp();

        ///  @todo help手册
        printf("******************************\n");
    }

    ///  连接服务器 并 设置非阻塞套接字
    cfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton(argv[1], &addr.sin_addr);
    addr.sin_port     = htons(atoi(argv[2]));
    socklen_t addrlen = sizeof(addr);
    if (0 != connect(cfd, (struct sockaddr *)&addr, addrlen))
    {
        zlog_warn(cli, "connect error: %s", strerror(errno));
        exit(-1);
    }
    else
    {
        zlog_debug(cli, "connect success");
    }

    printf("登录请输入你的用户名\n");
    printf("注册请输入你想要的用户名\n");
    printf("(不超过15个字符)\n");
    char username[20];
    char password[20];
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));
    scanf("%16s", username);
    /// login
    if (cli_accessusername(username))
    {
        int errornumber = 0;
    again:
        printf("请输入密码:\n");
        scanf("%19s", password);

        if (accesspassword(password))
        {
            if (useronline(username))
            {
                printf("您的账号已在别处下线\n");
            }

            printf("登录成功\n");
        }
        else
        {
            errornumber++;
            if (errornumber == 3)
            {
                printf(
                    "密码错误次数太多，暂时锁定帐号%s,"
                    "一分钟以后重新登陆\n",
                    username);
                exit(0);
            }
            else
            {
                printf("密码错误\n");
                goto again;
            }
        }
    }
    else  ///  注册
    {
        printf("-----------------注册:\n用户名:%s\n密码:", username);
        scanf("%19s", password);

        /// mysql

        printf("-----------------注册成功\n");
        printf("您的用户名\n%s\n您的密码\n%s\n请妥善保管\n", username,
               password);
    }

    showmainmenu();
}