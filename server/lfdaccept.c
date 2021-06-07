#include "config.h"
#include PROJECT_SERVERHEAD

extern zlog_category_t *ser;
extern events g_events[MAXCLIENT + 1];

void lfdaccept(int a, int b, void *args)
{
    events *lfdevent = args;
    int lfd          = lfdevent->fd;
    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen = sizeof(struct sockaddr_in);
    a                       = 1;  // 设置复用选项
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &a, (socklen_t)sizeof(int));
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &a, (socklen_t)sizeof(int));

    // accept
reaccept:
    int i   = 0;
    int cfd = 0;
    {
        cfd = accept(lfd, (struct sockaddr *)&clientaddr,
                     &clientaddrlen);  //非阻塞accept
        if (cfd < 0)
        {
            if (errno == EWOULDBLOCK)
            {
                zlog_warn(ser, "accept failed");
                goto reaccept;
            }
            else
            {
                zlog_warn(ser, "accper failed :%s", strerror(errno));
                return;
            }
        }

        for (; i < MAXCLIENT; i++)
        {
            if (g_events[i].status == 0) break;
        }
        if (i == MAXCLIENT)
        {
            zlog_warn(ser, "ERROR[BUSY] no found free events");
            return;
        }
    }

    // 设置cfd非阻塞
    {
        fcntl(cfd, F_SETFL, fcntl(cfd, F_GETFL, 0) | O_NONBLOCK);

        epoll_set(&g_events[i], cfd, client_event, &g_events[i]);
        epoll_add(EPOLLIN, &g_events[i]);
    }

    zlog_debug(ser,
               "\n\n-----------new client-------------\ncfd=%d from [%s:%d], "
               "g_events[%d]\n\n------------------------\n",
               cfd, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port),
               i);
    return;
}