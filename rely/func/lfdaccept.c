#include "../../include/chat.h"
extern int epfd;
extern bool PRINTEXIT;
extern bool DEBUGPRINT;
extern bool WRITE_LOG;
extern events g_events[MAXCLIENT + 1];
extern char serverlogpath[30];
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
        LOG(serverlogpath, "ERROR[BUSY] no found free events");
        return;
    }

    //cfd非阻塞
    //fcntl(cfd, F_SETFL, fcntl(cfd, F_GETFL, 0) | O_NONBLOCK);
    epoll_set(&g_events[i], cfd, client_event, &g_events[i]);
    epoll_add(EPOLLIN, &g_events[i]);
    LOG(serverlogpath, "\n\n-----------new client-------------\ncfd=%d from [%s:%d], g_events[%d]\n\n------------------------\n", cfd, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), i);
    return;
}