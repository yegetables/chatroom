
#include "config.h"
#include PROJECT_SERVERHEAD

extern int port;
extern int epfd;
extern events g_events[MAXCLIENT + 1];

void event_del(events *ev)
{
    if (ev->status == 0)
        return;
    else
        ev->status = 0;
    struct epoll_event temp = {0, {0}};
    temp.data.ptr           = NULL;
    // epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, NULL);
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &temp);
}