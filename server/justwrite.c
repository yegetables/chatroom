
#include "config.h"
#include PROJECT_SERVERHEAD

extern int epfd;
extern zlog_category_t *ser;

void justwrite(int cfd, int event, void *args)
{
    // 记录返回值
    int returnnumber = 0;
    // 记录错误次数
    int errornumber = 0;

    events *ev = (events *)args;
    // 构造info结构
    info *ms = &(ev->js);
    strcat(ms->value, "getted\n");
    // 发送
    {
        errornumber = 0;
    resend:
        if (sizeof(info) != (returnnumber = send(cfd, ms, sizeof(info), 0)))
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
            {
                errornumber++;
                zlog_warn(ser, "send sql failed:%s ,resending~~~~~",
                          strerror(errno));
                if (errornumber > 3)
                {
                    zlog_error(ser, "send sql failed %s ", strerror(errno));
                    return;
                }
                sleep(rand() % 2);
                goto resend;
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
    showevents(ev, __LINE__, __FILE__);  //写出的内容

    ev->call_back = client_event;
    epoll_add(EPOLLIN, ev);
    return;
}