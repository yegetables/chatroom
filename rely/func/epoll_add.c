#include "../../include/chat.h"
extern int epfd;
extern bool PRINTEXIT;
extern bool DEBUGPRINT;
extern bool WRITE_LOG;
extern char serverlogpath[30];
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
         LOG(serverlogpath,"event add error");
    }
    return;
}