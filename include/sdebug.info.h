#ifndef MYINCLUDE
#define MYINCLUDE
#endif
// #define _GNU_SOURCE
#include <arpa/inet.h>
#include <assert.h>
#include <bits/sockaddr.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <libgen.h>
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
#define move_up(nums) printf("\r\033[%dA", nums);
#define move_down(nums) printf("\r\033[%dB", nums);
#define move_left(nums) printf("\033[%dC", nums);
#define move_right(nums) printf("\033[%dD", nums);
#define move_save printf("\033[s");   // 保存光标位置
#define move_reset printf("\033[u");  //恢复光标位置
#define move_cll printf("\r\033[K");  //清行
#define move_cls printf("\033[2J");   //清屏

#define DEBUGPRINT(FORMAT, ...)                                                                 \
    {                                                                                           \
        char *t = strrchr(__FILE__, '/');                                                       \
        printf("[%s]", t);                                                                      \
        printf("[%s][%d]" YELLOW FORMAT NONECOLOR "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
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

#define PAUSE                                 \
    {                                         \
        fflush(stdout);                       \
        char *mm = readline("   任意键返回"); \
        fflush(stdout);                       \
        free(mm);                             \
        show_line++;                          \
    }
