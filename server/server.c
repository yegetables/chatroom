#include "../include/chat.h"
extern int port;
/* SELF */
int epfd;
events g_events[MAXCLIENT + 1];

int main(int argc, char **argv)
{
    //解析命令行
    char *options = "h";
    int opt;
    while ((opt = getopt(argc, argv, options)) != -1)
    { /*
        optarg —— 指向当前选项参数(如果有)的指针。
        optind —— 再次调用 getopt() 时的下一个 argv指针的索引。
        optopt —— 最后一个未知选项。
        opterr ­—— 如果不希望getopt()打印出错信息，则只要将全域变量opterr设为0即可。
        */
        if (opt == 'h')
            serverhelp();
        //TODO:help手册
        printf("******************************\n");
    }

    //读配置
    setconfig();
    printf("pid:%d\nserver port:%d\n", getpid(), port);

    //守护进程
    creat_daemon();

    //开日志
    //TODO:rm *.log
    system("rm ./log/*.log");
    zlog_category_t *ser = my_zlog_init("server");
    zlog_info(ser, "--------start--------");
    zlog_info(ser, " pid[%d]   port[%d]", getpid(), port);
    zlog_debug(ser, " epfd[%d]", epfd);

    exit(-1);
    //3.epoll反应堆模型
    epfd = epoll_create(MAXCLIENT);
    //TODO:多线程
    int lfd = epoll_init_lfd();
    zlog_debug(ser, "lfd:%d", lfd);

    struct epoll_event tempevents[MAXCLIENT];

    while (1)
    {
        // LOG(serverlogpath,"server running\n");
        int cmax = epoll_wait(epfd, tempevents, MAXCLIENT, 0);
        // if (cmax && DEBUGPRINT)
        //      LOG(serverlogpath,"wait up %d ", cmax);
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

    //4.状态
    /*
    while (1)
    {
        printf("server running\n");
        sleep(10);
    }*/
    zlog_fini();
    return 0;
}