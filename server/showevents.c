

#include "config.h"
#include PROJECT_SERVERHEAD

extern zlog_category_t *ser;
void showevents(events *ev, int line, char *fun)
{
    char logbuf[2 * BUFLEN];
    memset(logbuf, 0, sizeof(logbuf));
    // 函数,行数
    {
        fun = 1 + strrchr(fun, '/');
        sprintf(&logbuf[strlen(logbuf)], "\n-------events------\n[%s][%3d]",
                fun, line);
    }

    // events 基本信息
    {
        sprintf(&logbuf[strlen(logbuf)], "fd:[%d]", ev->fd);
        if (ev->events & EPOLLIN) sprintf(&logbuf[strlen(logbuf)], "[IN]");
        if (ev->events & EPOLLOUT) sprintf(&logbuf[strlen(logbuf)], "[OUT]");
        if (ev->events & EPOLLERR) sprintf(&logbuf[strlen(logbuf)], "[ERR]");
        if (ev->call_back == justwrite)
            sprintf(&logbuf[strlen(logbuf)], "[justwrite]");
        if (ev->call_back == client_event)
            sprintf(&logbuf[strlen(logbuf)], "[client_event]");
        if (ev->call_back == lfdaccept)
            sprintf(&logbuf[strlen(logbuf)], "[lfdaccept]");
        if (ev->status)
            sprintf(&logbuf[strlen(logbuf)], "live");
        else
            sprintf(&logbuf[strlen(logbuf)], "dead");
    }

    // info 信息
    {
        sprintf(&logbuf[strlen(logbuf)], "info:{\nfrom :%d to :%d   \n",
                ev->js.from, ev->js.to);

        if (ev->js.type == msg) sprintf(&logbuf[strlen(logbuf)], "[msg]");
        if (ev->js.type == file) sprintf(&logbuf[strlen(logbuf)], "[file]");
        if (ev->js.type == sql) sprintf(&logbuf[strlen(logbuf)], "[sql]");
        sprintf(&logbuf[strlen(logbuf)], "%s\n}", ev->js.value);
    }
    sprintf(&logbuf[strlen(logbuf)], "\n-----------------over---------\n");
    zlog_debug(ser, "%s", logbuf);
    return;
}