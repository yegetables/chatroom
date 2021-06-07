#include "config.h"
#include PROJECT_SERVERHEAD

extern zlog_category_t *ser;
extern int epfd;

void client_event(int cfd, int event, void *args)
{
    events *ev = args;
    info *ms   = &(ev->js);
    // 记录返回值
    int returnnumber = 0;
    // 记录错误次数
    int errornumber = 0;

    // 接受info
    {
        memset(ms, 0, sizeof(info));
        errornumber = 0;
    rerecv:
        if ((returnnumber = recv(cfd, ms, sizeof(info), 0)) != sizeof(info))
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
            {
                errornumber++;
                zlog_warn(ser,
                          "recv cfd_info failed %d times :%s rerecving~~~~~",
                          errornumber, strerror(errno));
                if (errornumber > 3)
                {
                    zlog_error(ser, "recv cfd_info failed");
                    goto over;
                }
                sleep(rand() % 2);
                goto rerecv;
            }
            else
            {
            over:
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
        // file/sql/cmd
        if (ev->js.type == sql)
        {
            //查询,判断
            ;
        }
        else
        {
            ;  //@todo js.type
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