
#include "../../include/chat.h"
extern int epfd;
extern bool PRINTEXIT;
extern bool DEBUGPRINT;
extern bool WRITE_LOG;
extern char serverlogpath[30];
void justwrite(int cfd, int event, void *args)
{

    events *ev = (events *)args;

    strcat(ev->buf, "getted\n");
    int len = 0;

    if ((len = write(cfd, ev->buf, strlen(ev->buf))) > 0)
    {
        event_del(ev);
        showevents(ev,__LINE__,__FUNCTION__);

        ev->len = 0;

        //更改buf  函数
        // LOG(serverlogpath,"client ev->fd %d  send:%s", ev->fd, ev->buf);

        // write(cfd, ev->buf, len);
        ev->call_back = client_event;
        bzero(ev->buf, sizeof(ev->buf));
        //epoll_set(ev, cfd, client_event, ev);
        epoll_add(EPOLLIN, ev);
    }
    else
    {
        close(cfd);
        LOG(serverlogpath, "close cfd:%d ", cfd);
    }
    return;
}