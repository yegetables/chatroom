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
                                                    \
  if (DEBUGPRINT)                                   \
  {                                                 \
    char *t = strrchr(__FILE__, '/');               \
    printf("[%s]", t);                              \
    printf("[%s][%d]" YELLOW FORMAT NONECOLOR "\n", \
           __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
  }

#define PRINTEXIT(FORMAT, ...)                                       \
  if (PRINTEXIT)                                                     \
  {                                                                  \
    char *t = strrchr(__FILE__, '/');                                \
    fprintf(stderr, "[%s]", t);                                      \
    fprintf(stderr, "[%s][%d]" YELLOW FORMAT NONECOLOR ":%s"         \
                    "\n",                                            \
            __FUNCTION__, __LINE__, ##__VA_ARGS__, strerror(errno)); \
    exit(-1);                                                        \
  }

#define WRITE_LOG(filename, FORMAT, ...)                                                        \
  if (WRITE_LOG)                                                                                \
  {                                                                                             \
    char *t = strrchr(__FILE__, '/');                                                           \
    FILE *fp = fopen(filename, "a+");                                                           \
    struct tm *pt;                                                                              \
    time_t timer;                                                                               \
    time(&timer);                                                                               \
    pt = localtime(&timer);                                                                     \
    fprintf(fp, "[%04d-%02d-%02d][%02d:%02d:%02d][%s][%s][%2d]" YELLOW FORMAT NONECOLOR "\r\n", \
            pt->tm_year + 1900,                                                                 \
            pt->tm_mon + 1,                                                                     \
            pt->tm_mday,                                                                        \
            pt->tm_hour,                                                                        \
            pt->tm_min,                                                                         \
            pt->tm_sec,                                                                         \
            t,                                                                                  \
            __FUNCTION__,                                                                       \
            __LINE__,                                                                           \
            ##__VA_ARGS__);                                                                     \
    fclose(fp);                                                                                 \
  }

#define LOG(filename, FORMAT, ...)                                               \
  {                                                                              \
    if (WRITE_LOG)                                                               \
    {                                                                            \
      char *t = strrchr(__FILE__, '/');                                          \
      int fp = open(filename, O_WRONLY | O_APPEND | O_CREAT | O_NONBLOCK, 0644); \
      struct tm *pt;                                                             \
      time_t timer;                                                              \
      time(&timer);                                                              \
      pt = localtime(&timer);                                                    \
      char logstring[1024];                                                      \
      memset(logstring, 0, sizeof(logstring));                                   \
      sprintf(logstring,                                                         \
              "[%02d:%02d][%s][%3d]" FORMAT                                      \
              "\r\n",                                                            \
              pt->tm_min,                                                        \
              pt->tm_sec,                                                        \
              __FUNCTION__,                                                      \
              __LINE__,                                                          \
              ##__VA_ARGS__);                                                    \
      write(fp, logstring, strlen(logstring));                                   \
      close(fp);                                                                 \
    }                                                                            \
  }