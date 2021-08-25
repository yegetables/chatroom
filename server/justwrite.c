#include "config.h"
#include PROJECT_SERVERHEAD

extern int epfd;
extern zlog_category_t *ser;
extern char fd_name[MAXCLIENT][30];
extern int fd_id[MAXCLIENT];
void justwrite(int cfd, int event, void *args)
{
    events *ev = (events *)args;
    info *ms = &ev->js;
    // 发送
    epoll_del(ev);
    int what_to;
    int returnnumber = -1;
    returnnumber = id_to_fd(ms->to);
    if (returnnumber != -1)
    {
        what_to = returnnumber;
    }
    else
    {  //未登录,使用ev->fd
        what_to = ev->fd;
    }

    if (!send_info(what_to, ms))
    {
        epoll_add(EPOLLRDHUP, ev);
        return;
    }

    //回调,事件,返回
    if (ms->how == SEND_FILE_REDY && atoi(ms->value) == 1)  // c -> s file
    {
        ev->call_back = IN_recvfile;
        epoll_add(EPOLLIN, ev);
        return;
    }
    else if (ms->how == AGREE_RECV_FILE)
    {
        ev->call_back = OUT_sendfile;
        epoll_add(EPOLLOUT, ev);
        return;
    }
    else
    {
        ev->call_back = client_event;
        epoll_add(EPOLLIN, ev);
        return;
    }

    return;
}
int id_to_fd(int id)
{
    if (id > 1)
    {
        int i = 0;
        for (; i < MAXCLIENT; i++)
        {
            if (fd_id[i] == id) return i;
        }
        if (i == MAXCLIENT) return -1;  //未登录
    }
    return -1;
}

int name_to_fd(char *name)
{
    int id = name_to_id(name);
    return id_to_fd(id);
}