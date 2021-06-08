#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;

void signalcatch(int signal)
{
    // 退出信号
    switch (signal)
    {
#ifdef SIGINT
        case SIGINT:
#endif
#ifdef SIGQUIT
        case SIGQUIT:
#endif
            zlog_debug(
                cli, "catch signal %s exit",
                show_signal(signal) == NULL ? "NULL" : show_signal(signal));
            exit(1);
#ifdef SIGCLD
        case SIGCLD:
#endif
            zlog_debug(
                cli, "catch signal %s return",
                show_signal(signal) == NULL ? "NULL" : show_signal(signal));
    }
}