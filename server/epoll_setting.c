#include "config.h"
#include PROJECT_SERVERHEAD
extern int epfd;
extern zlog_category_t *ser;
extern int port;
extern events g_events[MAXCLIENT + 1];

void epoll_add(int event, events *ev)
{
    struct epoll_event temp = {0, {0}};
    int op                  = EPOLL_CTL_DEL;
    temp.data.ptr           = ev;
    temp.events = ev->events = event;
    if (ev->status == 0)
    {
        op         = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    if (epoll_ctl(epfd, op, ev->fd, &temp) < 0)
    {
        zlog_error(ser, "event add error");
    }
}

void epoll_set(events *ev, int fd, void (*call_back)(int, int, void *),
               void *arg)
{
    ev->fd        = fd;
    ev->call_back = call_back;
    ev->arg       = arg;
    ev->events    = 0;
    ev->status    = 0;
    memset(&(ev->js), 0, sizeof(info));
    // ev->last_active = time(NULL);
    return;
}

void event_del(events *ev)
{
    if (ev->status == 0)
        return;
    else
        ev->status = 0;
    // struct epoll_event temp = {0, {0}};
    // temp.data.ptr           = NULL;
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, NULL);
    // epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &temp);
}

int epoll_init_lfd(void)
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    // lfd非阻塞
    //[为什么TCP服务器的监听套接字要设置为非阻塞](https://blog.csdn.net/zhwenx3/article/details/88107428)
    //如果监听套接字为非阻塞的，accept()不会阻塞住，立即返回-1，同时errno =
    // EWOULDBLOCK
    fcntl(lfd, F_SETFL, fcntl(lfd, F_GETFL, 0) | O_NONBLOCK);

    struct sockaddr_in serveraddr;
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port        = htons(port);

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