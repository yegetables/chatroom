#include "config.h"
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
extern int epfd;
extern char fd_name[MAXCLIENT][30];
extern int fd_id[MAXCLIENT];
void client_event(int cfd, int event, void *args)
{
    events *ev = args;
    // 接受info
    epoll_del(ev);
    if (false == recv_info(ev->fd, &(ev->js)))
    {
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
        zlog_info(ser, "**online**   fd:%d id:%d name:%s", ev->fd,
                  fd_id[ev->fd], fd_name[ev->fd]);
    }

    zlog_debug(ser, "========VVV=========");
    zlog_debug(ser, "========VVV=========");
    zlog_debug(ser, "========VVV=========");
    zlog_debug(ser, "========VVV=========");
    // TODO: client事件类别处理
    if (ev->js.to == 0)  // c/s交互
    {
        // sql/cmd
        if (ev->js.type == sql)
        {
            //查询,判断
            if (do_sql(ev))
            {
                if (ev->js.type != HUP_NO)
                {
                    ev->call_back = justwrite;
                    epoll_add(EPOLLOUT, ev);
                }
                else  //不回复
                {
                    epoll_add(EPOLLIN, ev);
                }
            }
            else  // 执行出错
            {
                epoll_add(EPOLLRDHUP, ev);
            }
        }
        else
        {
            if (ev->js.type == sql && ev->js.type == HUP_NO)
                // cli添加好友(放入requests队列不返回)
                ;  // js.type
            else
                ;
        }
    }
    else  // 转发 c/c交互
    {
        if (ev->js.how == ADD_FRIEND)  // /添加好友
        {
            if (ser_add_friend(ev))
            {
                //继续IN client_events
                epoll_add(EPOLLIN, ev);
            }
            else  // 执行出错
                epoll_add(EPOLLRDHUP, ev);

        }

        ;  // file/msg
    }
    p = showevents(ev);
    zlog_debug(ser, "to:%s", p);
    free(p);

    return;
}