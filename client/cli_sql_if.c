#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
extern int cfd;

bool cli_sql_if(char* p)
{
    if (p == NULL) return false;
    // 记录返回值
    int returnnumber = 0;
    // 记录错误次数
    int errornumber = 0;

    // 构造info结构
    info ms;
    {
        memset(&ms, 0, sizeof(ms));
        strcpy(ms.value, p);
        ms.type = sql;
        ms.from = -1;
        ms.to   = 0;
    }

    // 发送查询sql
    {
        errornumber = 0;
    resend:
        if (sizeof(info) != (returnnumber = send(cfd, &ms, sizeof(info), 0)))
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
            {
                errornumber++;
                zlog_warn(cli, "send sql failed:%s ,resending~~~~~",
                          strerror(errno));
                if (errornumber > 3)
                {
                    zlog_error(cli, "send sql failed %s ", strerror(errno));
                    return false;
                }
                sleep(rand() % 2);
                goto resend;
            }
            else
            {
                zlog_error(cli, "send sql failed %s", strerror(errno));
                return false;
            }
        }
    }

    // 接受并验证结果
    {
        memset(&ms, 0, sizeof(ms));
        errornumber = 0;
    rerecv:
        if ((returnnumber = recv(cfd, &ms, sizeof(ms), 0)) != sizeof(info))
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
            {
                errornumber++;
                zlog_warn(cli, "recv sql-result failed ,rerecving~~~~~");
                if (errornumber > 3)
                {
                    zlog_error(cli, "recv sql-result failed");
                    return false;
                }
                sleep(rand() % 2);
                goto rerecv;
            }
            else
            {
                zlog_error(cli, "recv sql failed %s", strerror(errno));
                return false;
            }
        }
        else if (ms.value[0] == '1' && ms.type == sql)
            return true;
        else
            return false;
    }

    return false;
}