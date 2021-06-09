#include "config.h"
#include PROJECT_SERVERHEAD

extern zlog_category_t *ser;
extern int epfd;

void client_event(int cfd, int event, void *args)
{
    events *ev = args;

    // 接受info
    event_del(ev);
    if (false == recv_info(ev->fd, &(ev->js)))
    {
        //关闭连接清空
        close(ev->fd);
        ev->status = 0;
        zlog_error(ser, "recv cfd_info failed,close cfd:%d ", ev->fd);
        return;
    }
    // TODO: client事件类别处理
    zlog_debug(ser, "recv success:%s", showevents(ev));
    zlog_debug(ser, "========VVV=========");
    zlog_debug(ser, "========VVV=========");
    zlog_debug(ser, "========VVV=========");
    zlog_debug(ser, "========VVV=========");

    if (ev->js.to == 0)
    {
        // file/sql/cmd
        if (ev->js.type == sql)
        {
            //查询,判断
            if (sql_do(ev))
            {
                ev->call_back = justwrite;
                epoll_add(EPOLLOUT, ev);
            }
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
    // ev->call_back = justwrite;
    // epoll_add(EPOLLOUT, ev);
    zlog_debug(ser, "to:%s", showevents(ev));

    return;
}