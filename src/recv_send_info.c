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
        zlog_warn(tmp, "recv failed %s", show_errno());
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            // 服务端不能卡死,客户端close之后服务端收到EAGAIN,三次失败直接close
            // 客户端适时等待能提高准确率,永远重试
#ifdef PROJECT_CLIENT
            sleep(1);  // magic number
            goto rerecv;
#endif
        }
        errornumber++;
        if (errornumber > 3)
        {
            zlog_warn(tmp, "can't recv info over 3");
            goto over;
        }
        goto rerecv;

    over:
        zlog_error(tmp, "recv info failed %s:%s over", show_errno(),
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
        zlog_warn(tmp, "send failed %s", show_errno());
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
#ifdef PROJECT_CLIENT
            sleep(1);  // magic number
            goto resend;
#endif
        }
        errornumber++;
        if (errornumber > 3)
        {
            zlog_warn(tmp, "can't send info over 3");
            goto over;
        }
        goto resend;

    over:
        zlog_error(tmp, "send info failed %s:%s over", show_errno(),
                   strerror(errno));
        return false;
    }
    return true;
}