#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
#else
#include PROJECT_CLIENTHEAD
#endif

char *showevents(events *ev)
{
    char *logbuf = (char *)calloc(2 * BUFLEN, sizeof(char));

    // events 基本信息
    {
        sprintf(&logbuf[strlen(logbuf)], "fd:[%d]", ev->fd);
        if (ev->events & EPOLLIN) sprintf(&logbuf[strlen(logbuf)], "[IN]");
        if (ev->events & EPOLLOUT) sprintf(&logbuf[strlen(logbuf)], "[OUT]");
        if (ev->events & EPOLLERR) sprintf(&logbuf[strlen(logbuf)], "[ERR]");
        if (ev->events & EPOLLRDHUP) sprintf(&logbuf[strlen(logbuf)], "[HUP]");
        if (ev->call_back == justwrite)
            sprintf(&logbuf[strlen(logbuf)], "[justwrite]");
        if (ev->call_back == client_event)
            sprintf(&logbuf[strlen(logbuf)], "[client_event]");
        if (ev->call_back == lfdaccept)
            sprintf(&logbuf[strlen(logbuf)], "[lfdaccept]");
        if (ev->status)
            sprintf(&logbuf[strlen(logbuf)], "[live]\n");
        else
            sprintf(&logbuf[strlen(logbuf)], "[dead]\n");
    }

    char *ii = showinfo(&(ev->js));
    strcpy(&logbuf[strlen(logbuf)], ii);
    free(ii);

    sprintf(&logbuf[strlen(logbuf)], "\n-----------------over---------\n");

    return logbuf;
}