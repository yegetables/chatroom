# 项目介绍
[![Commitizen friendly](https://img.shields.io/badge/commitizen-friendly-brightgreen.svg)](http://commitizen.github.io/cz-cli/)

## 服务端

使用 epoll 反应堆模型
		LT 水平触发模式
		mariadb 数据库
		cjson 配置文件

readline 实现目录补全效果

client 命令行可选指定端口



1. 读配置，写进全局变量
2. 解析命令行    配置自定义端口
3.  
4.  
5. 

## 客户端

### 特点: 

1. 使用 `zlog` 分日志级别 来 记录运行时状态

2. 屏蔽了常见的几种`信号`

3. 使用getopt函数来处理客户端`启动参数`(指定服务器地址和端口)

4. `非阻塞方式`使用socketfd 与服务器通信,在基本通信过程中 增加了 失败重试,超时返回报错 

5. 使用show_line 全局变量记录当前已显示的行数 ,  封装了 `printf` 的特殊方法实现`屏幕光标的移动`

	```c
   #define move_up(nums) printf("\r\033[%dA", nums);
   #define move_down(nums) printf("\r\033[%dB", nums);
   #define move_left(nums) printf("\033[%dC", nums);
   #define move_right(nums) printf("\033[%dD", nums);
   #define move_save printf("\033[s");   // 保存光标位置
   #define move_reset printf("\033[u");  //恢复光标位置
   #define move_cll printf("\r\033[K");  //清行
   #define move_cls printf("\033[2J");   //清屏
   ```

6.  实现`getch`, 修复了密码输入时删除的占位符显示问题

7.  实现了同一账号被第二次登陆时,第一次登录中的用户`被挤下线`功能

8.  正常登陆后(非聊天窗口) ,  会创建一个detach的子线程,定时 按顺序获得(更新锁和收发消息锁),查询 未读消息( 包括未处理的好友申请,文件未接受数量及来源,消息未读数量及来源) , 更新全局的消息数组,提供给主菜单固定的信息栏显示![image-20210831221358435](https://tu.yegetables.com/images/2021/08/31/20210831221358.png)

9. 包装了 通信函数(recv 与 send ),使得可以被服务端和客户端 使用   通信方式为传输固定大小的`结构体info`,内含一个buffer ,客户端方面 对于每次请求基本上都是 先发再收 所以使用 一个 临时的epoll_wait来处理in事件和out事件
   ```c
   typedef struct
   {
       char value[BUFLEN];  // value 缓冲区
       value_type type;     // type 枚举type(msg/file/sql/request)
       int from;            // from 客户端用户uid(-1->未登录用户)
       int to;              // to 接收者uid(0->服务器)
       int how;             // 宏-请求
   } info;                  // 定义info信息,用于网络传输的基本结构
   ```

   ![image-20210831221821165](https://tu.yegetables.com/images/2021/08/31/20210831221821.png)

   这些是how字段的内容
	```c
   info *cli_send_recv(info *ms, int how){
    tempevents.events = EPOLLOUT;
    epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tempevents);
    
    int thisnum = epoll_wait(epfd, outevents, 1024, 0);
    if (outevents[0].events & EPOLLIN){
         ret = recv(cfd, ms, sizeof(info), MSG_WAITALL);
    }else if (outevents[0].events & EPOLLOUT){
         ret = send(cfd, ms, sizeof(info), 0);
          tempevents.events = EPOLLIN;
            epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tempevents);
     }
   }
   ```
   
   ```c
   info *cli_creatinfo(int from, int to, value_type type, int how, char *value)
   {
       info *ms = (info *)malloc(sizeof(info));
       ms->how = how;
       ms->type = type;
   	strcpy(ms->value, value);
       ms->from = from;
       ms->to = to;
       pthread_mutex_lock(&rs_mutex);
       ms = cli_send_recv(ms, how);
       if (ms->how == SEND_FILE_REDY || ms->how == AGREE_RECV_FILE)
           ;
       else
           pthread_mutex_unlock(&rs_mutex);
       return ms;
   }
   ```
   
   处理客户端的底层收发细节     实现了将繁琐的收发细节抽象成了info的构建

10. 




### 遗憾

1. 未能实现 smtp的邮件重置密码功能
2. 未能做到 程序的输入与输出窗口分离,只做到了显示分离
3.  



![image-20210831215158821](https://tu.yegetables.com/images/2021/08/31/20210831215205.png)

client.c      ------>              定义了几个全局变量{    username ,userid, update_mutex  ,  rs_mutex , show_line }





















# 使用

**仅在 linux 下测试，不保证全平台通用**

## 1.安装

    cjson
    mysql