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
 * 好友name,userid,status,shield,(messages number)
 *  FR_LIST->base_GET_MANY_VALUE
 */
void cli_show_friends(void);

/**
 * @brief 输入对方id发送添加请求
 * IF_DONE ->add(ADD_FRIEND)
 */
void cli_add_friend(void);

/**
 * @brief 输入对方id 删除好友
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
 * @brief 更新applications与messages与files
 * @param 消息来源存入数组
 * @param who_send_msg 消息来源数组
 * @param who_send_files 文件来源数组
 */
void update_notices(char *user_msg, char *user_files);
/**
 * @brief 输入id或name
 * 返回name 或 id
 */
void cli_search_user(void);

/**
 * @brief 进入好友申请管理
 */
void show_applicationss_menu(void);

/**
 * @brief 同意或拒绝申请
 * @param is 0拒绝1同意
 */
void cli_agree_friend(int is);

/**
 * @brief 同意或拒绝文件
 * @param toid
 * @param is 0拒绝1同意
 */
void cli_recv_file(int toid, int is);

/**
 * @brief 接受或拒绝文件
 *
 * @param toid
 */
void show_file_menu(int toid);

/**
 * @brief 显示toid发送的文件们
 * @param 对方id
 */
void show_apply_files(int toid);

/**
 * @brief 查看好友申请信息
 * 仅能查看向自己
 * 不能查看自己向别人的的申请状态
 * SHOW_APPLY->search(ADD_FRIEND)
 * base_GET_MANY_VALUE
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

/**
 * @brief 私聊选项界面(查看历史,发文件,私聊)
 */
void show_secret_chat_menu(void);

/**
 * @brief 向toid发一条私聊信息
 * 数据库加一条 MESSAGES
 * @param toid 对方id
 * @return 0返回非0继续聊天
 */
int send_secret_message(int toid);

/**
 * @brief 接收toid私聊消息(所有未读)
 * @param toid 对方id
 */
void recv_secret_message(int toid);

/**
 * @brief toid私聊消息历史
 * @param toid 对方id
 */
void show_secret_message(int toid);

/**
 * @brief 定时刷新recv未读,随时send
 * @param toid
 */
void message_menu(int toid);

/**
 * @brief 包括发送文件请求和发送文件到服务器
 * 对方确认接受从服务器接受
 * @param toid 对方id
 * 只在数据库添加文件信息和路径
 */
void send_file_menu(int toid);

/**
 * @brief Get the file size object
 * @param path 路径
 * @return long int 失败-1
 */
long int get_file_size(char *path);
