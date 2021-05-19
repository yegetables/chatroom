#include "../include/ser.h"

extern int port;
extern char serverip[30];
extern int epfd;
extern events g_events[MAXCLIENT + 1];
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
    // inet_pton(AF_INET, serverip, &serveraddr.sin_addr);
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