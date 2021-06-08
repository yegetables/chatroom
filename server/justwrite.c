#include "config.h"
#include PROJECT_SERVERHEAD

extern int epfd;
extern zlog_category_t *ser;

void justwrite(int cfd, int event, void *args)
{
    events *ev = (events *)args;
    info *ms   = &(ev->js);
    // 发送
    if (!send_info(cfd, ms))
    {
        close(cfd);
        memset(ev, 0, sizeof(events));
        zlog_debug(ser, "close cfd:%d ", cfd);
        return;
    }
    event_del(ev);
    char *tt = showevents(ev);  //写出的内容
    strcat(tt, "has sended\n");
    zlog_debug(ser, tt);
    free(tt);

    ev->call_back = client_event;
    // memset(&(ev->js), 0, sizeof(info));
    epoll_add(EPOLLIN, ev);
}