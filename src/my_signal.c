#include "config.h"
#if defined PROJECT_CLIENT
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
#else
#include PROJECT_SERVERHEAD
extern zlog_category_t* ser;
#endif

void my_signal(int signal)
{
    char* sig = NULL;
    // 退出信号
    if (signal == SIGINT) sig = "SIGINT";
    if (signal == SIGQUIT) sig = "SIGQUIT";
    goto over;
    // 不退出信号
    if (signal == SIGCLD) sig = "SIGCLD";
#if defined PROJECT_CLIENT
    zlog_debug(cli, "catch signal %s return", sig);
#else
    zlog_debug(ser, "catch signal %s return", sig);
#endif
    return;
over:
#if defined PROJECT_CLIENT
    zlog_debug(cli, "catch signal %s exit", sig);
#else
    zlog_debug(ser, "catch signal %s exit", sig);
#endif
    exit(1);
}