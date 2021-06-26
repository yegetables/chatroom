#include <sys/sendfile.h>
#include <cjson/cJSON.h>
#include <mysql/mysql.h>
#include "sdebug.info.h"
#include "wrap.h"
#include <zlog.h>

#define BUFLEN 4096
#define MAXCLIENT 5000
/*
#if defined _WIN64 || defined _WIN32
#define CLREAR "cls"
#elif defined __linux
#define CLREAR "clear"
#endif
*/
#define HUP_NO 0
// 是否存在
#define IF_HAS 1
// 是否执行成功
#define IF_DONE 2
// 查到多少行
#define MANY_RESULT 3
// 第一行第一列值是多少
#define WHAT_FIRST_VALUE 4

//消息来源(单一列名字)
#define GET_MESSAGE_FROM 14
// 列出好友列表
#define FR_LIST 6
// 列出群组聊表
#define GR_LIST 20
//列出申请名单
#define SHOW_APPLY 9
//列出未读消息
#define SHOW_MESSAGES 13
//获取几行几列
#define GET_MANY_VALUE 10

// 设置登录状态
#define SET_ONLINE 5
// 同意好友申请
#define AGREE_APPLICATION 11
// 客户同意文件接收,服务器可发
#define AGREE_RECV_FILE 17
//查看群组申请
#define SHOW_GROUP_APPLY 25 	
//添加好友
#define ADD_FRIEND 7
//删除群组
#define DEL_GROUP 19
//更改权限
#define set_POWER_GROUP 23
//创建群组
#define CREATE_GROUP 18
//退出群组
#define EXIT_GROUP 22
//申请加群
#define ADD_GROUP_APPLY 21
//同意加群
#define ADD_GROUP 24

//转发消息
#define MESSAGES 8
//即将发送文件
#define SEND_FILE_REDY 15
//发送文件
#define SEND_FILE 16
// MARK:删除匹配信息
#define DEL_SELECT 12

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
#define NO_PEOPLE 0
//屏蔽
#define SHIELD 8
//不屏蔽
#define NO_SHIELD 0

typedef enum {
	msg = 1, // msg 文本消息(包括数值)
	file, // file 文件传输
	sql, // sql 数据库语句
	request // 发送请求
} value_type; // 枚举消息类型

typedef struct {
	char value[BUFLEN]; // value 缓冲区
	value_type type; // type 枚举type(msg/file/sql)
	int from; // from 客户端用户uid(-1->未登录用户)
	int to; // to 接收者uid(0->服务器)
	int how; // 宏-请求
} info; // 定义info信息,用于网络传输的基本结构

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
 * @brief 从user_id 转换user_name
 * @param id user_id
 * @param name  user_name
 * @return char* user_name
 */
char *id_to_name(int id, char *name);

/**
 * @brief 从user_name 转换 user_id
 * @param name user_name
 * @return int user_id
 */
int name_to_id(char *name);

/**
 * @brief 发送文件到cfd
 * @param cfd 接收方fd
 * @param pathname 文件上传路径
 * @param f_size 文件大小
 * @return true false
 */
bool send_file(int cfd, char *pathname, long int f_size);

/**
 * @brief 接受文件从cfd
 * @param cfd 文件来源
 * @param pathname 文件保存路径
 * @param f_size 文件大小
 * @return true false
 */
bool recv_file(int cfd, char *pathname, long int f_size);
