#include "cJSON.h"
#include "mysql/mysql.h"
#include "sdebug.info.h"
#include "wrap.h"
#include "zlog.h"

#define BUFLEN 4096
#define MAXCLIENT 5000

//是否存在
#define IF_HAS (1 << 0)
//是否执行成功
#define IF_DONE (1 << 1)
//查到多少
#define MANY_RESULT (1 << 2)
// //注册
// #define REGISTER 3
// //
// #define ACCESSNAME 1
// //
// #define ACCESSPASSWD 2
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
    int how;             //宏-请求
} info;                  // 定义info信息,用于网络传输的基本结构

/**
 * @brief 帮助手册
 */
void help(void);

/**
 * @brief 初始化zlog的分类
 * @param category  分类名称
 * @return zlog_category_t* 分类句柄
 */
zlog_category_t *my_zlog_init(char *category);

/**
 * @brief 根据errno转换成被定义的宏
 * @param errno
 * @return char* 宏(例如EINTR)
 */
char *show_errno(void);

/**
 * @brief 根据signal转换成信号名称
 * @param signal 信号
 * @return char* 对应信号名称
 */
char *show_signal(int signal);

/**
 * @brief 显示info信息
 * @param ms info指针
 * @return char* 待写入日志
 */
char *showinfo(info *ms);

/**
 * @brief 接受fd的info存入ms
 * @param cfd 指定fd
 * @param ms (传出)指向存入位置
 * @return true
 * @return false
 */
bool recv_info(int cfd, info *ms);

/**
 * @brief 发送ms的info到fd
 * @param cfd 指定fd
 * @param ms (传入)指向待传出位置
 * @return true
 * @return false
 */
bool send_info(int cfd, info *ms);

/**
 * @brief 数字转字符串
 *
 * @param num 数字
 * @param str 传出字符串指针
 * @param radix 进制
 * @return char* str指针
 */
char *itoa(int num, char *str, int radix);

/**
 * @brief 验证邮箱格式
 * @param s 邮箱字符串
 * @return true false
 */
bool IsValidEmail(char *s);