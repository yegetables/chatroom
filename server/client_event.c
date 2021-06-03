#include "config.h"
#include PROJECT_SERVERHEAD

extern zlog_category_t *ser;
extern int epfd;

void client_event(int cfd, int event, void *args)
{
    events *ev = (events *)args;
    // IN
    //可能是非阻塞

    // while (1)

    if (sizeof(info) != recv(ev->fd, &(ev->js), sizeof(info), 0))
    {
        zlog_warn(ser, "recv info failed");
        memset(&(ev->js), 0, sizeof(info));
        close(cfd);
        zlog_debug(ser, "close cfd:%d ", cfd);
        return;
        //@todo: fix errno
    }

    event_del(ev);
    if (ev->js.to == 0)
    {
        if (ev->js.type == sql) return;
    }
    ev->call_back = justwrite;

    showevents(ev, __LINE__, __FILE__);
    epoll_add(EPOLLOUT, ev);

    // epoll_set(ev, cfd, write1, ev);

    return;
}