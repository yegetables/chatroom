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
        zlog_error(tmp, "recv failed %s", show_errno());
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            goto rerecv;
        }
        errornumber++;
        if (errornumber > 3)
        {
            zlog_error(tmp, "can't recv info over 3");
            goto over;
        }
        goto rerecv;

    over:
        zlog_warn(tmp, "recv info failed %s:%s over", show_errno(),
                  strerror(errno));
        return false;
    }
    return true;
}

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
        if (errno == EWOULDBLOCK || errno == EAGAIN) goto resend;

        errornumber++;
        if (errornumber > 3)
        {
            goto over;
        }
        goto resend;

    over:
        zlog_warn(tmp, "send info failed %s:%s over", show_errno(),
                  strerror(errno));
        return false;
    }
    return true;
}