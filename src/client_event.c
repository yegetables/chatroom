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
    if ((ev->len = read(cfd, ev->buf, BUFLEN)) > 0)
    {
        event_del(ev);
        // ev->len--;
        // ev->buf[ev->len] = 0;

        // write(cfd, ev->buf, len);
        // epoll_set(ev, cfd, justwrite, ev);
        ev->call_back = justwrite;

        showevents(ev, __LINE__, __FILE__);
        epoll_add(EPOLLOUT, ev);
    }
    else
    {
        close(cfd);
        zlog_debug(ser, "close cfd:%d ", cfd);
    }

    // epoll_set(ev, cfd, write1, ev);

    return;
}