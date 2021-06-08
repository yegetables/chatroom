#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
#else
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
#endif
bool send_info(int cfd, info *ms)
{
    zlog_category_t *tmp = NULL;
#ifdef PROJECT_SERVER
    tmp = ser;
#else
    tmp = cli;
#endif
    // 记录返回值
    int returnnumber = 0;
    // 记录错误次数
    int errornumber = 0;

resend:
    if (sizeof(info) != (returnnumber = send(cfd, ms, sizeof(info), 0)))
    {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
        {
            errornumber++;
            if (errornumber > 3)
            {
                goto over;
            }
            goto resend;
        }
        else
        {
        over:
            zlog_warn(tmp, "send info failed %s:%s over", show_errno(),
                      strerror(errno));
            return false;
        }
    }
    return true;
}