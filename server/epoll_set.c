#include "config.h"
#include PROJECT_SERVERHEAD

void epoll_set(events *ev, int fd, void (*call_back)(int, int, void *),
               void *arg)
{
    ev->fd        = fd;
    ev->call_back = call_back;
    ev->arg       = arg;
    ev->events    = 0;
    ev->status    = 0;
    memset(&(ev->js), 0, sizeof(info));
    // ev->last_active = time(NULL);
    return;
}