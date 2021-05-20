
#include "config.h"
#include PROJECT_SERVERHEAD

extern int epfd;
extern zlog_category_t *ser;
void justwrite(int cfd, int event, void *args)
{
    events *ev = (events *)args;
    strcat(ev->buf, "getted\n");
    ev->len = strlen(ev->buf);
    int len = 0;

    if ((len = write(cfd, ev->buf, ev->len)) > 0)
    {
        event_del(ev);
        showevents(ev, __LINE__, __FILE__);

        ev->len = 0;

        //更改buf  函数
        // LOG(serverlogpath,"client ev->fd %d  send:%s", ev->fd, ev->buf);

        // write(cfd, ev->buf, len);
        ev->call_back = client_event;
        bzero(ev->buf, sizeof(ev->buf));
        // epoll_set(ev, cfd, client_event, ev);
        epoll_add(EPOLLIN, ev);
    }
    else if (len < 0)
    {
        close(cfd);
        zlog_error(ser, "close cfd:%d ", cfd);
    }
    return;
}