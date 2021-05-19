#include "../include/ser.h"
extern int epfd;
extern zlog_category_t *ser;
extern events g_events[MAXCLIENT + 1];

void lfdaccept(int a, int b, void *args)
{
    events *lfdevent = (events *)args;
    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen = sizeof(struct sockaddr_in);
    //lfd非阻塞
    int cfd = accept(lfdevent->fd, (struct sockaddr *)&clientaddr, &clientaddrlen);
    if (cfd == -1)
    {
        if (errno == EINTR || errno == EAGAIN)
        {
            PRINTEXIT("accept");
        }
        else
            PRINTEXIT("accept");
        return;
    }
    int i = 0;
    for (; i < MAXCLIENT; i++)
    {
        if (g_events[i].status == 0)
            break;
    }
    if (i == MAXCLIENT)
    {
        zlog_warn(ser, "ERROR[BUSY] no found free events");
        return;
    }

    //cfd非阻塞
    //fcntl(cfd, F_SETFL, fcntl(cfd, F_GETFL, 0) | O_NONBLOCK);
    epoll_set(&g_events[i], cfd, client_event, &g_events[i]);
    epoll_add(EPOLLIN, &g_events[i]);
    zlog_debug(ser, "\n\n-----------new client-------------\ncfd=%d from [%s:%d], g_events[%d]\n\n------------------------\n", cfd, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), i);
    return;
}