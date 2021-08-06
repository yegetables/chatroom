#include "config.h"
#include PROJECT_SERVERHEAD

char database_name[20];
char database_user[20];
char database_passwd[100];
char database_ip[20];
int database_port = 3306;

extern int epfd;
extern zlog_category_t *ser;
extern int port;
extern events g_events[MAXCLIENT + 1];

void epoll_add(int event, events *ev)
{
    struct epoll_event temp = {0, {0}};
    int op = EPOLL_CTL_DEL;
    temp.data.ptr = ev;
    temp.events = ev->events = event;
    if (ev->status == 0)
    {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    if (epoll_ctl(epfd, op, ev->fd, &temp) < 0)
    {
        zlog_error(ser, "event add error");
    }
}

void epoll_set(events *ev, int fd, void (*call_back)(int, int, void *), void *arg)
{
    ev->fd = fd;
    ev->call_back = call_back;
    ev->arg = arg;
    ev->events = 0;
    ev->status = 0;
    memset(&ev->js, 0, sizeof(info));
    // ev->last_active = time(NULL);
    return;
}

void epoll_del(events *ev)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, NULL);
    if (ev->status == 0)
    {
        return;
    }
    else
    {
        ev->status = 0;
    }
    // struct epoll_event temp = {0, {0}};
    // temp.data.ptr           = NULL;

    // epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &temp);
}

int epoll_init_lfd(void)
{
    int lfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    // lfd非阻塞
    //[为什么TCP服务器的监听套接字要设置为非阻塞](https://blog.csdn.net/zhwenx3/article/details/88107428)
    //如果监听套接字为非阻塞的，accept()不会阻塞住，立即返回-1，同时errno =
    // EWOULDBLOCK
    // fcntl(lfd, F_SETFL, fcntl(lfd, F_GETFL, 0) | O_NONBLOCK);

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);

    //端口复用
    int op = 1;
    // TCP不支持多播
    // setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &op, sizeof(op));

    bind(lfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(lfd, MAXCLIENT);

    /*不考虑多线程*/
    epoll_set(&g_events[MAXCLIENT], lfd, lfdaccept,
              &g_events[MAXCLIENT]);           //设置回调
    epoll_add(EPOLLIN, &g_events[MAXCLIENT]);  // ev->fd

    return lfd;
}

void ser_setconfig(char *filename)
{
    int configfd;
    char bufjson[BUFLEN] = {0};

    //读配置文件
    if (!access(filename, F_OK) && !access(filename, R_OK))
    {
        configfd = open(filename, O_RDONLY);
        int n = 1;
        while (n != 0)
        {
            n = read(configfd, bufjson, BUFLEN);
        }
        n = 0;
        {
            cJSON *configjson = cJSON_Parse(bufjson);
            if (configjson == NULL)
            {
                perror("not json ");
                exit(-1);
            }

            cJSON *item = NULL;
            char *name = NULL;
            name = "server_port";
            if (cJSON_HasObjectItem(configjson, name) &&
                cJSON_IsNumber((item = cJSON_GetObjectItem(configjson, name))))
            {
                port = item->valueint;
            }
            name = "database_port";
            if (cJSON_HasObjectItem(configjson, name) &&
                cJSON_IsNumber((item = cJSON_GetObjectItem(configjson, name))))
            {
                database_port = item->valueint;
            }

            // string
            {
                name = "database_passwd";
                if (cJSON_HasObjectItem(configjson, name) &&
                    cJSON_IsString((item = cJSON_GetObjectItem(configjson, name))))
                {
                    strcpy(database_passwd, item->valuestring);
                }

                name = "database_name";
                if (cJSON_HasObjectItem(configjson, name) &&
                    cJSON_IsString((item = cJSON_GetObjectItem(configjson, name))))
                {
                    strcpy(database_name, item->valuestring);
                }
                name = "database_user";
                if (cJSON_HasObjectItem(configjson, name) &&
                    cJSON_IsString((item = cJSON_GetObjectItem(configjson, name))))
                {
                    strcpy(database_user, item->valuestring);
                }
                name = "database_ip";
                if (cJSON_HasObjectItem(configjson, name) &&
                    cJSON_IsString((item = cJSON_GetObjectItem(configjson, name))))
                {
                    strcpy(database_ip, item->valuestring);
                }
            }

            cJSON_Delete(configjson);
        }

        close(configfd);
    }
    else
    {
        perror("config.json can't read ");
        exit(-1);
    }
}

char *showevents(events *ev)
{
    char *logbuf = (char *)calloc(2 * BUFLEN, sizeof(char));

    // events 基本信息
    {
        sprintf(&logbuf[strlen(logbuf)], "[fd:%d]", ev->fd);
        if (ev->events & EPOLLRDHUP)
        {
            sprintf(&logbuf[strlen(logbuf)], "[HUP]");
        }
        else
        {
            if (ev->events & EPOLLERR)
            {
                sprintf(&logbuf[strlen(logbuf)], "[ERR]");
            }
            else
            {
                if (ev->events & EPOLLIN)
                {
                    sprintf(&logbuf[strlen(logbuf)], "[IN]");
                }
                else
                {
                    if (ev->events & EPOLLOUT)
                    {
                        sprintf(&logbuf[strlen(logbuf)], "[OUT]");
                    }
                }
            }
        }
        if (ev->call_back == justwrite) sprintf(&logbuf[strlen(logbuf)], "[justwrite]");
        if (ev->call_back == client_event) sprintf(&logbuf[strlen(logbuf)], "[client_event]");
        if (ev->call_back == lfdaccept) sprintf(&logbuf[strlen(logbuf)], "[lfdaccept]");
        if (ev->status)
        {
            sprintf(&logbuf[strlen(logbuf)], "[live]\n");
        }
        else
        {
            sprintf(&logbuf[strlen(logbuf)], "[dead]\n");
        }
    }

    char *ii = showinfo(&ev->js);
    strcpy(&logbuf[strlen(logbuf)], ii);
    free(ii);

    sprintf(&logbuf[strlen(logbuf)], "\n-----------------over---------\n");

    return logbuf;
}

int creat_daemon(void)
{
    // 1.创建子进程，父进程退出
    pid_t pid = fork();
    if (pid == 0)
    {
        // 2.创建新会话,使子进程完全独立出来，脱离控制
        setsid();

        //再次创建子进程(第二次保证进程不是进程组长)
        pid = fork();
        if (pid == 0)
        {
            ;
        }
        else if (pid == -1)
        {
            perror("fork");
            return -1;
        }
        else
        {
            exit(0);
        }
    }
    else if (pid == -1)
    {
        perror("fork");
        return -1;
    }
    else
    {
        exit(0);
    }

    printf("pid:%d\n", getpid());

    // 3.切换目录
    // chdir("/");

    // 4.重设文件权限掩码
    // umask(0);

    // 5.关文件
    for (int i = 0; i < NOFILE; close(i++))
    {
        ;
    }

    // 6.信号处理
    //如果对于信号处理通过怕生成子进程执行,要设置信号忽略SIGCHLD信号,可以避免子进程成为僵尸
    signal(SIGCHLD, SIG_IGN);

    // 7.其他,可以处理别的
    //也可以返回主程序,执行任务,此时主程序成为守护进程

    return 0;
}

void lfdaccept(int a, int b, void *args)
{
    events *lfdevent = (events *)args;
    int lfd = lfdevent->fd;
    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen = sizeof(struct sockaddr_in);
    a = 1;  // 设置复用选项
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &a, (socklen_t)sizeof(int));
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &a, (socklen_t)sizeof(int));

    // accept
reaccept:;
    int i = 0;
    int cfd = 0;
    {
        cfd = accept(lfd, (struct sockaddr *)&clientaddr,
                     &clientaddrlen);  //非阻塞accept
        if (cfd < 0)
        {
            if (errno == EWOULDBLOCK)
            {
                zlog_warn(ser, "accept failed");
                goto reaccept;
            }
            else
            {
                zlog_error(ser, "accpet failed :%s", strerror(errno));
                return;
            }
        }

        for (; i < MAXCLIENT; i++)
        {
            if (g_events[i].status == 0) break;
        }
        if (i == MAXCLIENT)
        {
            zlog_warn(ser, "ERROR[BUSY] no found free events");
            return;
        }
    }

    // 设置cfd非阻塞
    {
        // fcntl(cfd, F_SETFL, fcntl(cfd, F_GETFL, 0) | O_NONBLOCK);

        epoll_set(&g_events[i], cfd, client_event, &g_events[i]);
        epoll_add(EPOLLIN, &g_events[i]);
    }

    zlog_info(ser,
              "\n-----------new client-------------\ncfd=%d from [%s:%d], "
              "g_events[%d]\n------------------------",
              cfd, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), i);
    return;
}
