#include "common.h"

/**
 * @brief 设置how并收发
 * @param info* ms构建from,to,type,value
 * @param how 消息功效
 * @return info*接受的info
 */
info *cli_send_recv(info *ms, int how);

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
bool cli_accessonline(char *name);

/**
 * @brief 设置账号在线
 * @param name 用户名
 * @return true  false
 */
int cli_setonline(char *name);

/**
 * @brief 发送注册信息
 * @param username 用户名
 * @param passwd 密码
 * @param email 邮箱
 * @return true 注册成功
 * false 注册失败
 */
bool cli_register(char *username, char *passwd, char *email);

/**
 * @brief 显示功能列表
 */
void show_main_menu(void);

/**
 * @brief 显示好友管理界面
 */
void show_management_friends_menu(void);

/**
 * @brief 获取好友列表
 * 好友name,userid,status
 */
void cli_show_friends();

/**
 * @brief 发送添加请求
 * 输入对方id
 */
void cli_add_friend();

/**
 * @brief 删除好友
 * 输入对方id
 */
void cli_del_friend(void);

/**
 * @brief 屏蔽好友
 * 输入对方id
 * @param is 1->屏蔽
 * 0->取消屏蔽
 */
void cli_shield_friend(int is);

/**
 * @brief 固定信息栏,在任何界面都可更新
 */
void information_bar(void);
/**
 * @brief 更新applications与messages
 */
void update_notices(void);

/**
 * @brief 输入id或name
 * 返回name 或 id
 * 以及在线状态
 */
void cli_search_user(void);

/**
 * @brief 查看好友申请信息
 * 仅能查看向自己
 * 不能查看自己向别人的的申请状态
 */
void show_apply_friends(void);

/**
 * @brief 信号捕捉
 * SIG_IGN 终止
 * @param signal 对应信号
 */
void signalcatch(int signal);

/**
 * @brief 验证邮箱格式
 * @param s 邮箱字符串
 * @return true false
 */
bool IsValidEmail(char *s);
