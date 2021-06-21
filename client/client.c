#include "config.h"
#include PROJECT_CLIENTHEAD
char username[30];
int userid;
#define SLEEP_TIME 3
zlog_category_t *cli = NULL;
int cfd;
int main(int argc, char **argv)
{
    srand((unsigned)time(NULL));
    userid = -1;
    // 记录错误次数
    int errornumber = 0;
    // 记录返回值
    int returnnumber = 0;

    /// 开日志
    {
        cli = my_zlog_init("client");
        zlog_info(cli, "--------start--------");
    }

    /// 注册信号捕捉函数
    {
        signal(SIGQUIT, signalcatch);
        signal(SIGINT, signalcatch);
    }

    /// 解析命令行
    {
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

            if (opt == 'h') help();

            ///  @todo help手册
            printf("******************************\n");
        }
        if (argc < 2)
        {
            printf("usage: ./client ipaddr port \n");
            exit(1);
        }
    }

    /// 连接服务器
    {
        cfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        // addr.sin_addr.s_addr = htonl(INADDR_ANY);
        inet_aton(argv[1], &addr.sin_addr);
        addr.sin_port     = htons(atoi(argv[2]));
        socklen_t addrlen = sizeof(addr);

        errornumber = 0;
    reconnect:;
        if (-1 ==
            (returnnumber = connect(cfd, (struct sockaddr *)&addr, addrlen)))
        {
            sleep(rand() % 3);
            zlog_debug(cli, "connect failed :%s,reconnecting~~~~",
                       strerror(errno));
            if (errornumber > 3)
            {
                sleep(rand() % 3);
                errornumber = 0;
                if (-1 == connect(cfd, (struct sockaddr *)&addr, addrlen))
                {
                    zlog_debug(cli, "connect error %s", strerror(errno));
                    exit(-1);
                }
            }
            else
            {
                errornumber++;
                goto reconnect;
            }
        }
        zlog_info(cli, "connect success");
    }

    /// 登录或者注册
    {
        printf("登录请输入你的用户名\n");
        printf("注册请输入你想要的用户名\n");
        printf("(不超过15个字符)\n");
        char passwd[30];
        char email[50];
        memset(username, 0, sizeof(username));
        memset(passwd, 0, sizeof(passwd));
        memset(email, 0, sizeof(email));
        fgets(username, 25, stdin);
        {
            if (username[strlen(username) - 2] == '\r' &&
                username[strlen(username) - 1] == '\n')
                username[strlen(username) - 2] = '\0';
            if (username[strlen(username) - 1] == '\n')
                username[strlen(username) - 1] = '\0';
        }
        /// login
        if (cli_accessusername(username))
        {
            zlog_info(cli, "login %s", username);
            errornumber = 0;
        again:;

            printf("请输入密码:\n");
            printf("忘记密码请输入#forget#username#email\n");

            fgets(passwd, 25, stdin);
            {
                if (passwd[strlen(passwd) - 2] == '\r' &&
                    passwd[strlen(passwd) - 1] == '\n')
                    passwd[strlen(passwd) - 2] = '\0';
                if (passwd[strlen(passwd) - 1] == '\n')
                    passwd[strlen(passwd) - 1] = '\0';
            }

            if (strncmp(passwd, "#forget#", 8) == 0 &&
                IsValidEmail(&passwd[17]))
            {
                // TODO: 重置密码
                // send email
                // scanf 验证码
                // 重置 密码
                printf("您的用户名\n%s\n您的密码\n%s\n请妥善保管\n", username,
                       passwd);
                zlog_info(cli, "重置密码成功:用户名:%s 密码:%s", username,
                          passwd);
            }

            if (cli_accesspasswd(username, passwd))
            {
                zlog_info(cli, "login %s passwd right passwd:%s", username,
                          passwd);
                if (cli_accessonline(username))
                    printf("您的账号已在别处下线\n");
                zlog_info(cli, "login success");
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
                    zlog_warn(cli,
                              "login %s passwd error passwd error 3 times ",
                              username);
                    exit(0);
                }
                else
                {
                    zlog_debug(cli, "login %s passwd error  %d times ",
                               username, errornumber);
                    printf("密码错误\n");
                    goto again;
                }
            }
        }
        else  ///  注册
        {
            zlog_info(cli, "register user: %s", username);
            printf("-----------------注册:\n");
            printf("用户名:%s\n", username);
            printf("密码开头不能是#,不超过24位\n");
            printf("密码:");
            fgets(passwd, 25, stdin);
            {
                if (passwd[strlen(passwd) - 2] == '\r' &&
                    passwd[strlen(passwd) - 1] == '\n')
                    passwd[strlen(passwd) - 2] = '\0';
                if (passwd[strlen(passwd) - 1] == '\n')
                    passwd[strlen(passwd) - 1] = '\0';
            }
            printf("输入验证邮箱:\n");
            printf("邮箱不超过48位\n");
            fgets(email, 50, stdin);
            {
                if (email[strlen(email) - 2] == '\r' &&
                    email[strlen(email) - 1] == '\n')
                    email[strlen(email) - 2] = '\0';
                if (email[strlen(email) - 1] == '\n')
                    email[strlen(email) - 1] = '\0';
            }  /// mysql add register
            errornumber = 0;
            while (false == cli_register(username, passwd, email) &&
                   errornumber++ < 3)
                sleep(rand() % SLEEP_TIME);

            if (errornumber >= 3)
            {
                zlog_warn(cli, "register failed 用户名:%s 密码:%s 邮箱:%s",
                          username, passwd, email);
                exit(1);
            }
            printf("-----------------注册成功\n");
            printf("您的用户名:%s\n您的密码:%s\n您的邮箱:%s\n请妥善保管\n",
                   username, passwd, email);
            zlog_info(cli, "注册成功:用户名:%s 密码:%s 邮箱:%s", username,
                      passwd, email);
        }
    }

    // 进入功能菜单
    userid = cli_setonline(username);
    if (userid > 0)
        show_main_menu();
    else
        zlog_error(cli, "userid %d  error", userid);
    close(cfd);
    zlog_fini();
    return 0;
}

void signalcatch(int signal)
{
    // 退出信号
    switch (signal)
    {
#ifdef SIGINT
        case SIGINT:
#endif
#ifdef SIGQUIT
        case SIGQUIT:
#endif
            zlog_debug(cli, "catch signal %s exit", show_signal(signal));
            zlog_fini();
            close(cfd);
            exit(1);
#ifdef SIGCLD
        case SIGCLD:
#endif
            zlog_debug(cli, "catch signal %s return", show_signal(signal));
            zlog_fini();
            close(cfd);
            exit(1);
            // 异常退出
    }
}

