#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
extern int cfd;

bool cli_sql_if(char* p)
{
    if (p == NULL) return false;

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
    if (!send_info(cfd, &ms)) return false;  // no close

    // 接受并验证结果
    if (recv_info(cfd, &ms) && ms.value[0] == '1' && ms.type == sql)
        return true;
    else
    {
        zlog_debug(cli, "access failed");  // no close
        return false;
    }
}