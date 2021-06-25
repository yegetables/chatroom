#include "common.h"

// q->u
//禁言
#define NO_TALK 1
//不禁言
#define TALK 0

// u->q
//群主
#define OWNER 7
//管理员
#define MANER 3
//群员
#define PEOPLE 1
//未加入
#define NO_PEORPLE0
//屏蔽
#define SHIELD 8
//不屏蔽
#define NO_SHIELD 0

typedef struct {
	int from_id;
	long f_size;
	char path[PATH_MAX / 2];
	off_t offset;

} file_ready;
typedef struct {
	int fd; //  要监听的文件描述符
	int events; //  对应的监听事件
	void *arg; //  泛型参数
	void (*call_back)(int fd, int events, void *arg); //  回调函数
	int status; //  是否在监听:1->在红黑树上(监听), 0->不在(不监听)
	info js; // 消息结构
} events; // epoll事件的void*

/**
 * @brief 读取服务端配置
 * @param filename 文件名
 */
void ser_setconfig(char *filename);

/**
 * @brief 初始化events结构
 * 填入fd,回调
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
void epoll_del(events *ev);

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
 * @brief  先向info->to 写
 * 如果0/-1 向ev->fd写
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
bool do_sql(events *ev);

/**
 * @brief 构造uuid路径,和数据库
 * 消息,设置回调为IN_recvfile
 * @param ev
 * @return long int 文件大小
 */
long int event_recv_file_ready(events *ev);

/**
 * @brief 接受文件的epin事件回调函数
 * 写回IF_DONE,被ready的justwrite设置
 * 肯定是上一个要发文件的人的id
 * @param cfd
 * @param event
 * @param args
 */
void IN_recvfile(int cfd, int event, void *args);

/**
 * @brief 在线时根据uid找到fd
 */
int id_to_fd(int id);

/**
 * @brief 在线时根据name找到fd
 */
int name_to_fd(char *name);

/**
 * @brief 第一个值,char*放入buf
 * @return  执行状况 true false
 */
bool case_WHAT_FIRST_VALUE(info *ms);
/**
 * @brief 有多少行数据
 * @return  执行状况 true false
 */
bool case_MANY_RESULT(info *ms);
/**
 * @brief 是否存在
 * @return  执行状况 true false
 */
bool case_IF_HAS(info *ms);
/**
 * @brief 执行成功
 * @return  执行状况 true false
 */
bool case_IF_DONE(info *ms);

/**
 * @brief 执行buf的sql语句
自动记录报错
 * @param ms info
 * @return true  false
 */
bool base_sql_query(info *ms);

/**
 * @brief 设置列数,打印
 * number\nstr str\n
 * @param ms 结果存入buf
 * @param fetch 列数
 * @return int 行数
 */
int base_GET_MANY_VALUE(info *ms, int fetch);
/**
 * @brief 设置在线
 * 先设置status ,获取name,查询id返回
 * 顺便加入fd_id在线状态
 * @param events* ev
 * @return true false
 */
bool event_set_online(events *ev);

/**
 * @brief 获取好友列表
 * 好友name,userid
 * @param ms info
 * @return int number
 */
#define ser_show_friends(ms) base_GET_MANY_VALUE(ms, 2)
/**
 * @brief 获取好友申请列表
 * 好友name,userid
 * @param ms info
 * @return int number
 */
#define ser_show_apply(ms) base_GET_MANY_VALUE(ms, 2)

/**
 * @brief 处理添加好友
 * @param ev
 * @return true false
 * 处理完成
 */
bool ser_add_friend(events *ev);

/**
 * @brief 服务器发送文件
 * @param cfd
 * @param event
 * @param args
 */
void OUT_sendfile(int cfd, int event, void *args);

/**
 * @brief 准备处理文件接收
 *
 * @param ms
 * @return true
 * @return false
 */
bool event_AGREE_RECV_FILE(info *ms);

bool event_DEL_GROUP(info *ms);

bool event_CREATE_GROUP(info *ms);

void set_relationship(int id1, int id2, int is);

void del_relationship(int *id1, int *id2);
