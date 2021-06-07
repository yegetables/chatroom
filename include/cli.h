#include "common.h"

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

/**
 * @brief 信号捕捉
 * SIG_IGN 终止
 * @param signal 对应信号
 */
void signalcatch(int signal);
