

#include "config.h"
#include PROJECT_SERVERHEAD

extern zlog_category_t *ser;
void showevents(events *ev, int line, char *fun)
{
    //    events *ev = (events *)args;
    char logbuf[ev->len + 1 + BUFLEN];
    memset(logbuf, 0, sizeof(logbuf));
    fun = 1 + strrchr(fun, '/');
    sprintf(&logbuf[strlen(logbuf)], "\n-------events------\n[%s][%3d]", fun,
            line);
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
        sprintf(&logbuf[strlen(logbuf)], "true");
    else
        sprintf(&logbuf[strlen(logbuf)], "false");

    sprintf(&logbuf[strlen(logbuf)], "buf:[%s]", ev->buf);
    sprintf(&logbuf[strlen(logbuf)], "buflen:%d", ev->len);
    sprintf(&logbuf[strlen(logbuf)], "\n-----------------over---------\n");

    // printf("%s\n", logbuf);
    zlog_debug(ser, "%s", logbuf);
    return;
}