#include "config.h"
#include PROJECT_SERVERHEAD
extern zlog_category_t* ser;
extern MYSQL* sql_l;
int ser_show_friends(info* ms) { return base_GET_MANY_VALUE(ms, 2); }

int ser_show_apply(info* ms) { return base_GET_MANY_VALUE(ms, 2); }

bool ser_add_friend(events* ev)
{
    info* ms = &(ev->js);
    if (!base_sql_query(ms)) return false;  //添加进requests
    if (!case_IF_DONE(ms)) return false;    // 回复1
    return false;
}
