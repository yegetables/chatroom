#include "sdebug.info.h"
#include "wrap.h"
#include <zlog.h>
#include <mysql/mysql.h>
#include <cjson/cJSON.h>
#define BUFLEN 4096
#define MAXCLIENT 5000
typedef enum
{
    true = 1,
    false = 0
} bool;

typedef struct
{
    int fd;                                           //要监听的文件描述符
    int events;                                       //对应的监听事件
    void *arg;                                        //泛型参数
    void (*call_back)(int fd, int events, void *arg); //回调函数
    int status;                                       //是否在监听:1->在红黑树上(监听), 0->不在(不监听)
    char buf[BUFLEN];
    int len;
    long last_active; //记录每次加入红黑树 g_efd 的时间值

} events;

void setconfig(void);
void serverhelp(void);
void epoll_set(events *ev, int fd, void (*call_back)(int, int, void *), void *arg);
void epoll_add(int event, events *ev);
int epoll_init_lfd(void);

//cfd初始事件(lfd accept分派)read close
void client_event(int, int, void *args);
//
void justwrite(int, int, void *args);

void lfdaccept(int, int, void *args);
void event_del(events *ev);

void showevents(void *args, int line, char *fun);
int creat_daemon(void);
zlog_category_t *my_zlog_init(char *);
