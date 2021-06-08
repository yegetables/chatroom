#include "config.h"
#include PROJECT_SERVERHEAD

extern zlog_category_t *ser;
extern int epfd;

void client_event(int cfd, int event, void *args)
{
    events *ev = args;
    // 接受info
    if (false == recv_info(cfd, &(ev->js)))
    {
        //关闭连接清空
        close(cfd);
        memset(ev, 0, sizeof(events));
        zlog_debug(ser, "recv cfd_info failed,close cfd:%d ", cfd);
        return;
    }

    // TODO: client事件类别处理
    event_del(ev);

    if (ev->js.to == 0)
    {
        // file/sql/cmd
        if (ev->js.type == sql)
        {
            //查询,判断
            ;
        }
        else
        {
            ;  // js.type
        }
    }
    else  // 转发
    {
        ;  // file/msg
    }
    ev->call_back = justwrite;
    epoll_add(EPOLLOUT, ev);
    return;
}