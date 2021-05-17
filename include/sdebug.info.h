#ifndef MYINCLUDE
#define MYINCLUDE
#endif

#include <arpa/inet.h>
#include <bits/sockaddr.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <sys/un.h>
#include <stddef.h>
#include <sys/epoll.h>
#include <getopt.h>
#include <linux/limits.h>
#include "color.h"
#include <sys/param.h>
#include <syslog.h>

#define DEBUGPRINT(FORMAT, ...)                     \
  {                                                 \
    char *t = strrchr(__FILE__, '/');               \
    printf("[%s]", t);                              \
    printf("[%s][%d]" YELLOW FORMAT NONECOLOR "\n", \
           __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
  }

#define PRINTEXIT(FORMAT, ...)                                       \
  {                                                                  \
    char *t = strrchr(__FILE__, '/');                                \
    fprintf(stderr, "[%s]", t);                                      \
    fprintf(stderr, "[%s][%d]" YELLOW FORMAT NONECOLOR ":%s"         \
                    "\n",                                            \
            __FUNCTION__, __LINE__, ##__VA_ARGS__, strerror(errno)); \
    exit(-1);                                                        \
  }

