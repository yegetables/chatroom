
#include "../include/ser.h"

extern int port;
extern char serverip[30];
extern int epfd;
extern events g_events[MAXCLIENT + 1];

void event_del(events *ev)
{
    if (ev->status == 0)
        return;
    else
        ev->status = 0;
    struct epoll_event temp = {0, {0}};
    temp.data.ptr = NULL;
    //epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, NULL);
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &temp);

    return;
}