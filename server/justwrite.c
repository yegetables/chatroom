#include "config.h"
#include PROJECT_SERVERHEAD

extern int epfd;
extern zlog_category_t *ser;

void justwrite(int cfd, int event, void *args)
{
    events *ev = (events *)args;
    info *ms   = &(ev->js);
    // 发送
    event_del(ev);
    if (!send_info(cfd, ms))
    {
        epoll_add(EPOLLRDHUP, ev);
        return;
    }

    ev->call_back = client_event;
    memset(ms, 0, sizeof(info));
    epoll_add(EPOLLIN, ev);
}