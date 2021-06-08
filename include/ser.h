#include "common.h"

typedef struct
{
    int fd;      //  要监听的文件描述符
    int events;  //  对应的监听事件
    void *arg;   //  泛型参数
    void (*call_back)(int fd, int events, void *arg);  //  回调函数
    int status;  //  是否在监听:1->在红黑树上(监听), 0->不在(不监听)
    info js;  // 消息结构
} events;     // epoll事件的void*

/**
 * @brief 读取服务端配置
 * @param filename 文件名
 */
void setserconfig(char *filename);

/**
 * @brief 初始化events结构
 *填入fd,回调
 * @param ev 指定全局events结构
 * @param fd 对应的fd(lfd/cfd)
 * @param call_back
 * @param arg
 */
void epoll_set(events *ev, int fd, void (*call_back)(int, int, void *),
               void *arg);

/**
 * @brief 添加事件进入epfd
 *
 * @param event 事件(IN/OUT)
 * @param ev 对应全局events
 */
void epoll_add(int event, events *ev);

/**
 * @brief 取下events,设置status=0
 * @param ev 指定events
 */
void event_del(events *ev);

/**
 * @brief 初始化lfd事件加入epfd
 */
int epoll_init_lfd(void);

/**
 * @brief cfd初始事件
 * @param args 对应的全局events
 */
void client_event(int, int, void *args);

/**
 * @brief 测试向cfd写
 * @param ev 对应的全局events
 */
void justwrite(int, int, void *ev);

/**
 * @brief lfd负责accpet,
 * 初始添加cfd的events结构加入epfd
 * 指定回调为client_event
 * @param args lfd的全局结构
 */
void lfdaccept(int, int, void *args);

/**
 * @brief 展示events结构
 * @param args events结构
 * @return char* events信息
 */
char *showevents(events *args);

/**
 * @brief 创建守护进程
 */
int creat_daemon(void);

/**
 * @brief 连接mysql
 *
 * @return MYSQL* 句柄
 */
MYSQL *sql_connect(void);

/**
 * @brief 初始化mysql表:
 * user/related tables
 * @param sql句柄
 * @return true false
 */
bool sql_init_table(MYSQL *sql_l);

/**
 * @brief 处理type为sql的数据
 * @param ev events*
 * @return true 完成
 */
bool sql_do(events *ev);

// bool ser_accessname();