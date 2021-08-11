#include "config.h"
#include PROJECT_CLIENTHEAD
char username[30];
int userid;
#define SLEEP_TIME 3
zlog_category_t *cli = NULL;
int cfd;
int show_line = 0;
char who_send_msg[BUFLEN];
char who_send_file[BUFLEN];
int epfd;
struct epoll_event tempevents;
int main(int argc, char **argv)
{
    char *readusername = NULL;

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
        signal(SIGALRM, signalcatch);
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
             * @note optind —— 再次调用 getopt()时的下一个
             * argv指针的索引。
             * @note optopt —— 最后一个未知选项。
             * @note opterr ­——
             * 如果不希望getopt()打印出错信息，则只要将全域变量opterr设为0即可。
             */

            if (opt == 'h') help();

            ///  @todo help手册
            printf("******************************\n");
            show_line++;
        }
        if (argc < 2)
        {
            printf("usage: ./client ipaddr port \n");
            exit(1);
        }
    }

    /// 连接服务器
    {
        // cfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        cfd = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        // addr.sin_addr.s_addr = htonl(INADDR_ANY);
        inet_aton(argv[1], &addr.sin_addr);
        addr.sin_port = htons(atoi(argv[2]));
        socklen_t addrlen = sizeof(addr);

        errornumber = 0;
    reconnect:;
        if (-1 == (returnnumber = connect(cfd, (struct sockaddr *)&addr, addrlen)))
        {
            sleep(rand() % 3);
            zlog_debug(cli, "connect failed :%s,reconnecting~~~~", strerror(errno));
            if (errornumber > 30)
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
    //增加epoll 句柄
    epfd = epoll_create(10);
    tempevents.data.fd = cfd;

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
        // fgets(username, 25, stdin);

        readusername = readline("");
        strcpy(username, readusername);
        free(readusername);
        show_line += 4;
        {
            if (username[strlen(username) - 2] == '\r' && username[strlen(username) - 1] == '\n')
            {
                username[strlen(username) - 2] = '\0';
            }
            if (username[strlen(username) - 1] == '\n') username[strlen(username) - 1] = '\0';
        }
        /// login
        if (cli_accessusername(username))
        {
            zlog_info(cli, "login %s", username);
            errornumber = 0;
        again:;

            printf("请输入密码:\n");
            printf("忘记密码请输入#forget#username#email\n");
            show_line += 2;
            // fgets(passwd, 25, stdin);

            {
                int jj = 0;
                char c;
                while (1)
                {
                    c = getch();
                    // printf("%d--", (int)c);
                    if (c == '\r' || c == '\n')  //当读到回车符时输入结束
                    {
                        break;
                    }
                    passwd[jj++] = c;
                    // printf("*");
                }
                printf("\b \n");
                passwd[jj] = '\0';  //需要在字符的末尾补加字符串结束标志，才可以以字符串形式输出
                show_line++;
                // printf("beg:%s:end\n", passwd);
            }

            {
                if (passwd[strlen(passwd) - 2] == '\r' && passwd[strlen(passwd) - 1] == '\n')
                {
                    passwd[strlen(passwd) - 2] = '\0';
                }
                if (passwd[strlen(passwd) - 1] == '\n') passwd[strlen(passwd) - 1] = '\0';
            }

            if (strncmp(passwd, "#forget#", 8) == 0 && IsValidEmail(&passwd[17]))
            {
                // TODO(ajian): 重置密码
                // send email
                // scanf 验证码
                // 重置 密码
                printf("您的用户名\n%s\n您的密码\n%s\n请妥善保管\n", username, passwd);
                show_line += 5;
                zlog_info(cli, "重置密码成功:用户名:%s 密码:%s", username, passwd);
            }

            if (cli_accesspasswd(username, passwd))
            {
                zlog_info(cli, "login %s passwd right passwd:%s", username, passwd);
                if (cli_accessonline(username))
                {
                    show_line++;
                    printf("您的账号已在别处下线\n");
                }
                zlog_info(cli, "login success");
                printf("登录成功\n");
                show_line++;
            }
            else
            {
                errornumber++;
                if (errornumber == 30)
                {
                    printf(
                        "密码错误次数太多，暂时锁定帐号%s,"
                        "一分钟以后重新登陆\n",
                        username);
                    show_line++;
                    zlog_warn(cli, "login %s passwd error passwd error 3 times ", username);
                    exit(0);
                }
                else
                {
                    zlog_debug(cli, "login %s passwd error  %d times ", username, errornumber);
                    printf("密码错误\n");
                    show_line++;
                    goto again;
                }
            }
        }
        else  ///  注册
        {
            zlog_info(cli, "register user: %s", username);
        reinput:;
            show_line += 3;
            printf("-----------------注册:\n");
            printf("用户名:%s\n", username);
            printf("密码开头不能是#,不超过24位\n");
            printf("密码:");
            // fgets(passwd, 25, stdin);

            {
                int jj = 0;
                char c;
                while (1)
                {
                    c = getch();
                    // printf("%d--", (int)c);
                    if (c == '\r' || c == '\n')  //当读到回车符时输入结束
                    {
                        break;
                    }
                    passwd[jj++] = c;
                    // printf("*");
                }
                printf("\b \n");
                passwd[jj] = '\0';  //需要在字符的末尾补加字符串结束标志，才可以以字符串形式输出
                show_line += 2;
                // printf("beg:%s:end\n", passwd);
            }
            {
                if (passwd[strlen(passwd) - 2] == '\r' && passwd[strlen(passwd) - 1] == '\n')
                {
                    passwd[strlen(passwd) - 2] = '\0';
                }
                if (passwd[strlen(passwd) - 1] == '\n') passwd[strlen(passwd) - 1] = '\0';
            }
            if (passwd[0] == '#' || strlen(passwd) > 24)
            {
                printf("重新输入密码\n");
                show_line++;
                // break;
                goto reinput;
            }

            printf("输入验证邮箱:\n");
            printf("邮箱不超过48位\n");
            show_line += 3;
            fgets(email, 50, stdin);
            {
                if (email[strlen(email) - 2] == '\r' && email[strlen(email) - 1] == '\n')
                {
                    email[strlen(email) - 2] = '\0';
                }
                if (email[strlen(email) - 1] == '\n') email[strlen(email) - 1] = '\0';
            }  /// mysql add register
            errornumber = 0;
            while (false == cli_register(username, passwd, email) && errornumber++ < 30)
            {
                sleep(rand() % SLEEP_TIME);
            }

            if (errornumber >= 30)
            {
                zlog_warn(cli, "register failed 用户名:%s 密码:%s 邮箱:%s", username, passwd,
                          email);
                exit(1);
            }
            printf("-----------------注册成功\n");
            printf("您的用户名:%s\n您的密码:%s\n您的邮箱:%s\n请妥善保管\n", username, passwd,
                   email);
            printf("输入任意键登录");
            getchar();
            zlog_info(cli, "注册成功:用户名:%s 密码:%s 邮箱:%s", username, passwd, email);
            show_line += 5;
        }
    }

    // 进入功能菜单
    userid = cli_setonline(username);
    if (userid > 0)
    {
        // system("clear");
        update_notices(who_send_msg, who_send_file);
        // alarm(10);
        show_main_menu();
    }
    else
    {
        zlog_error(cli, "userid %d  error", userid);
    }
    close(cfd);
    zlog_fini();
    return 0;
}

void signalcatch(int signal)
{
    // 退出信号
    switch (signal)
    {
        case SIGINT:
        case SIGQUIT:
            zlog_debug(cli, "catch signal %s exit", show_signal(signal));
            zlog_fini();
            close(cfd);
            exit(1);
        case SIGALRM:
            // update_notices(who_send_msg, who_send_file);
            // alarm(10);
            return;
            // 异常退出
    }
}
