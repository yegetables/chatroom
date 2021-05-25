#include "config.h"
#include PROJECT_SERVERHEAD

int port = 500;  ///< var 服务器端口
int epfd;        ///< 全局epfd
events g_events[MAXCLIENT + 1];
zlog_category_t *ser = NULL;
extern char database_name[20];
int main(int argc, char **argv)
{
    char q[BUFLEN];  /// 数据库语句
    memset(q, 0, sizeof(q));

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

        if (opt == 'h') serverhelp();

        ///  @todo help手册
        printf("******************************\n");
    }

    /// @brief 读配置
    setserconfig(PROJECT_SERCONFIG);
    printf("server port:%d\n", port);

    /// @brief 守护进程
    creat_daemon();

    /// @brief 开日志
    char cmd[100] = {0};
#ifdef DEBUG
    sprintf(cmd, "rm ");
    strcat(cmd, PROJECT_LOGPATH);
    strcat(cmd, "sever*.log");
    system(cmd);
#endif
    ser = my_zlog_init("server");
    zlog_info(ser, "--------start--------");
    zlog_info(ser, "pid[%d]   port[%d]", getpid(), port);
    zlog_debug(ser, "epfd[%d]", epfd);

    MYSQL *sql = NULL;
    sql        = sql_connect();
    if (sql == NULL)
    {
        zlog_error(ser, "sql_connect error");
        exit(-1);
    }

    /// @brief epoll反应堆模型
    epfd = epoll_create(MAXCLIENT);

    ///  @todo 多线程
    int lfd = epoll_init_lfd();
    zlog_debug(ser, "lfd:%d", lfd);

    struct epoll_event tempevents[MAXCLIENT];

    while (1)
    {
        int cmax = epoll_wait(epfd, tempevents, MAXCLIENT, 0);
        for (int i = 0; i < cmax; i++)
        {
            events *this = tempevents[i].data.ptr;
            if (tempevents[i].events & EPOLLIN)
            {
                this->call_back(this->fd, tempevents[i].events, this->arg);
            }
            else if (tempevents[i].events & EPOLLOUT)
            {
                this->call_back(this->fd, tempevents[i].events, this->arg);
            }
        }
    }

    zlog_fini();
    mysql_close(sql);
    return 0;
}