#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
#else
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
#endif

bool recv_info(int cfd, info *ms)
{
    zlog_category_t *tmp = NULL;
#ifdef PROJECT_SERVER
    tmp = ser;
#else
    tmp = cli;
#endif

    memset(ms, 0, sizeof(info));
    // 记录返回值
    int returnnumber = 0;
    // 记录错误次数
    int errornumber = 0;
rerecv:
    if ((returnnumber = recv(cfd, ms, sizeof(info), 0)) != sizeof(info))
    {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
        {
            errornumber++;
            if (errornumber > 3)
            {
                goto over;
            }
            sleep(rand() % 2);
            goto rerecv;
        }
        else
        {
            zlog_warn(tmp, "recv info failed %s:%s over", show_errno(),
                      strerror(errno));
        over:
            return false;
        }
    }
    return true;
}
