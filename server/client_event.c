#include "config.h"
#include PROJECT_SERVERHEAD

extern zlog_category_t *ser;
extern int epfd;

void client_event(int cfd, int event, void *args)
{
    events *ev = args;
    info ms    = (ev->js);
    // 记录返回值
    int returnnumber = 0;
    // 记录错误次数
    int errornumber = 0;

    // 接受info
    {
        memset(&ms, 0, sizeof(ms));
        errornumber = 0;
    rerecv:
        if ((returnnumber = recv(cfd, &ms, sizeof(ms), 0)) != sizeof(info))
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
            {
                errornumber++;
                zlog_warn(ser, "recv cfd_info failed ,rerecving~~~~~");
                if (errornumber > 3)
                {
                    zlog_error(ser, "recv cfd_info failed");
                    return false;
                }
                sleep(rand() % 2);
                goto rerecv;
            }
            else
            {
                close(cfd);
                memset(ev, 0, sizeof(events));
                zlog_debug(ser, "close cfd:%d ", cfd);
                return;
            }
        }
    }

    event_del(ev);
    if (ev->js.to == 0)
    {
        if (ev->js.type == sql)

            //
            return;
    }
    ev->call_back = justwrite;
    showevents(ev, __LINE__, __FILE__);
    epoll_add(EPOLLOUT, ev);

    // epoll_set(ev, cfd, write1, ev);

    return;
}