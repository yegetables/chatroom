#include "cJSON.h"
#include "mysql/mysql.h"
#include "sdebug.info.h"
#include "wrap.h"
#include "zlog.h"
#define BUFLEN 4096
#define MAXCLIENT 5000

typedef enum
{
    msg,       // msg 文本消息
    file,      // file 文件传输
    sql        // sql 数据库语句
} value_type;  // 枚举消息类型

typedef struct
{
    char value[BUFLEN];  // value 缓冲区
    value_type type;     // type 枚举type(msg/file/sql)
    int from;            // from 客户端用户id(-1->未登录用户)
    int to;              // to 接收者id(0->服务器)
} info;                  // 定义info信息,用于网络传输的基本结构

/**
 * @brief client帮助手册
 */
void clienthelp(void);

/**
 * @brief 初始化zlog的分类
 * @param category  分类名称
 * @return zlog_category_t* 分类句柄
 */
zlog_category_t *my_zlog_init(char *category);

/**
 * @brief 构建sql语句并查询
 * @param sql_l sql语句
 * @return true
 * false
 */
bool cli_sql_if(char *sql_l);

/**
 * @brief 客户端发送待验证密码是否正确
 * @param name 输入的用户名
 * @param passwd 输入的密码
 * @return true 密码正确
 * false 密码错误
 */
bool cli_accesspasswd(char *name, char *passwd);

/**
 * @brief 客户端发送待验证用户名是否已存在
 * @param name 输入的用户名
 * @return true 存在user表中
 * false 不存在,需要注册
 */
#define cli_accessusername(name) cli_accesspasswd(name, NULL)

/**
 * @brief 验证账号是否已在线,
 * 若在线则挤掉
 * @param name 用户名
 * @return true 在线
 * false 不在线
 */
bool useronline(char *name);

/**
 * @brief 发送注册信息
 * @param username 用户名
 * @param passwd 密码
 * @return true 注册成功
 * false 注册失败
 */
bool cli_register(char *username, char *passwd);

/**
 * @brief 显示客户端主页
 */
void showmainmenu(void);

// void justwrite(int, int, void *args);

// void showevents(void *args, int line, char *fun);