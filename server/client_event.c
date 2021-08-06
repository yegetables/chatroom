#include "config.h"
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
extern int epfd;
extern char fd_name[MAXCLIENT][30];
extern int fd_id[MAXCLIENT];
void client_event(int cfd, int event, void *args)
{
    events *ev = (events *)args;
    // 接受info
    epoll_del(ev);
    if (false == recv_info(ev->fd, &ev->js))
    {
        zlog_debug(ser, "recv_info error,rdhup");
        epoll_add(EPOLLRDHUP, ev);
        return;
    }
    char *p = showevents(ev);
    zlog_debug(ser, "recv success:%s", p);
    free(p);
    if (ev->js.from > 0 && fd_id[ev->fd] != ev->js.from)  //添加进在线列表
    {
        fd_id[ev->fd] = ev->js.from;               // fd->uid
        id_to_name(ev->js.from, fd_name[ev->fd]);  // fd->name
        zlog_info(ser, "**online**   fd:%d id:%d name:%s", ev->fd, fd_id[ev->fd], fd_name[ev->fd]);
    }

    zlog_debug(ser, "========VVV=========");
    zlog_debug(ser, "========VVV=========");
    zlog_debug(ser, "========VVV=========");
    zlog_debug(ser, "========VVV=========");

    if (ev->js.how == HUP_NO)
    {
        do_sql(ev);
        memset(ev->js.value, 0, BUFLEN);
        epoll_add(EPOLLIN, ev);
        goto over;
    }

    // sql/cmd
    if (ev->js.type == sql || ev->js.type == msg)
    {
        //查询,判断
        if (do_sql(ev))
        {
            ev->call_back = justwrite;
            epoll_add(EPOLLOUT, ev);
        }
        else
        {
            epoll_add(EPOLLRDHUP, ev);
        }
        goto over;
    }

over:;
    p = showevents(ev);
    zlog_debug(ser, "to:%s", p);
    free(p);

    return;
}
