#ifndef MYINCLUDE
#define MYINCLUDE
#endif

#include <arpa/inet.h>
#include <assert.h>
#include <bits/sockaddr.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/limits.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include "color.h"


#define DEBUGPRINT(FORMAT, ...)                                       \
    {                                                                 \
        char *t = strrchr(__FILE__, '/');                             \
        printf("[%s]", t);                                            \
        printf("[%s][%d]" YELLOW FORMAT NONECOLOR "\n", __FUNCTION__, \
               __LINE__, ##__VA_ARGS__);                              \
    }

#define PRINTEXIT(FORMAT, ...)                                           \
    {                                                                    \
        char *t = strrchr(__FILE__, '/');                                \
        fprintf(stderr, "[%s]", t);                                      \
        fprintf(stderr,                                                  \
                "[%s][%d]" YELLOW FORMAT NONECOLOR                       \
                ":%s"                                                    \
                "\n",                                                    \
                __FUNCTION__, __LINE__, ##__VA_ARGS__, strerror(errno)); \
        exit(-1);                                                        \
    }
