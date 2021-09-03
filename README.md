# 聊天室

[![Commitizen friendly](https://img.shields.io/badge/commitizen-friendly-brightgreen.svg)](http://commitizen.github.io/cz-cli/)

## 服务端

1. 使用 epoll 反应堆模型

2. LT 水平触发模式

3. mariadb 数据库

4. json 配置文件(定义了 数据库的配置,端口等配置)

5. 守护进程

6. 事件处理包裹       {  epoll_event  ----->>>>      events     ------>>>>>>   info                          }

    ```c
    struct epoll_event
    {
      uint32_t events;	/* Epoll events */
      epoll_data_t data;	/* User data variable */
    } __EPOLL_PACKED;
    
    
    typedef union epoll_data
    {
      void *ptr;
      int fd;
      uint32_t u32;
      uint64_t u64;
    } epoll_data_t;
    
    typedef struct
    {
        int fd;                                            //  要监听的文件描述符
        int events;                                        //  对应的监听事件
        void *arg;                                         //  泛型参数
        void (*call_back)(int fd, int events, void *arg);  //  回调函数
        int status;  //  是否在监听:1->在红黑树上(监听), 0->不在(不监听)
        info js;     // 消息结构
    } events;        // epoll事件的void*
    
    typedef struct
    {
        char value[BUFLEN];  // value 缓冲区
        value_type type;     // type 枚举type(msg/file/sql)
        int from;            // from 客户端用户uid(-1->未登录用户)
        int to;              // to 接收者uid(0->服务器)
        int how;             // 宏-请求
    } info;                  // 定义info信息,用于网络传输的基本结构
    
    
    
    ```

    

7. 主程序 epoll wait  处理 rdhup和 call_back

    ```c
    //linstenfd  设置In事件  并且设置 callback 为 一个处理connect的函数   
    
    while (1)
    {
            int max = epoll_wait(epfd, tempevents, MAXCLIENT, 0);
            for (int i = 0; i < max; i++)
            {
    			if (tempevents[i].events & EPOLLRDHUP)
    			{
                    //错误处理
    				continue;
    			}
    			this->call_back(this->fd, this->events, this->arg);
            }
    }
    ```

    

8. 提前设置listenfd的事件

    ```c
     	bind(lfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
        listen(lfd, MAXCLIENT);
        epoll_set(&g_events[MAXCLIENT], lfd, lfdaccept,
                  &g_events[MAXCLIENT]);           //设置回调
        epoll_add(EPOLLIN, &g_events[MAXCLIENT]);
    
    //--------------------lfdaccept()-----------------------------
    cfd = accept();
    epoll_set(&g_events[i], cfd, client_event, &g_events[i]);
    epoll_add(EPOLLIN, &g_events[i]);
    
    ```

    

9. client_event

    ```c
    recv_info();
    //记录为在线用户   id  对应  fd
    do_sql(ev);
    ev->call_back = justwrite;
    epoll_add(EPOLLOUT, ev);
    return;
    ```

    

10.  do_sql()![image-20210903190822041](https://tu.yegetables.com/images/2021/09/03/20210903190822.png) 做一些数据库和业务操作

    ```c
     ms->to = ms->from;
     ms->from = 0;
     break;
    ```

    

    

11.  just_write

     ```c
     send_info();
     
     ev->call_back = client_event;
     epoll_add(EPOLLIN, ev);
     return;
     ```

     

12.  file部分则 为       

     ​     或者 

     ```c
        //回调,事件,返回
         if (ms->how == SEND_FILE_REDY && atoi(ms->value) == 1)  // c -> s file
         {
             ev->call_back = IN_recvfile;
             epoll_add(EPOLLIN, ev);
             return;
         }
         else if (ms->how == AGREE_RECV_FILE)
         {
             // switch检测到了之后   将 justwrite 换成另一个 sendfile和设置回调client event 的函数   
             ev->call_back = OUT_sendfile;
             epoll_add(EPOLLOUT, ev);
             return;
         }
         else
         {
             ev->call_back = client_event;
             epoll_add(EPOLLIN, ev);
             return;
         }
     
         return;
     ```

     

13. 

## 客户端

### 特点: 

1. 使用 `zlog` 分日志级别 来 记录运行时状态
2. 屏蔽了常见的几种`信号`
3. 使用getopt函数来处理客户端`启动参数`(指定服务器地址和端口)
4. readline 实现目录补全效果
5. `非阻塞方式`使用socketfd 与服务器通信,在基本通信过程中 增加了 失败重试,超时返回报错 
6. 定义了几个全局变量{    username ,userid, update_mutex  ,  rs_mutex , show_line }
7. 使用show_line 全局变量记录当前已显示的行数 ,  封装了 `printf` 的特殊方法实现`屏幕光标的移动`

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

8.  正常登陆后(非聊天窗口) ,  会创建一个detach的子线程,定时 按顺序获得(`up更新锁`和`rs收发消息锁`),查询 未读消息( 包括未处理的好友申请,文件未接受数量及来源,消息未读数量及来源) , 更新全局的消息数组,提供给主菜单固定的信息栏显示![image-20210903182359124](https://tu.yegetables.com/images/2021/09/03/20210903182359.png)

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

10.  void cli_show_friends(void) 进行了四次查询分别是
    ```c
            // 0->在线未屏蔽 10
            // 1->离线未屏蔽 00
            // 2->在线屏蔽 11
            // 3->离线屏蔽 01
    ```
    
11. 聊天时,创建了新的子线程来实时接受对方的消息并换行显示,实现了双方左右分屏聊天的效果
    ```c
    void secret_message_menu(int toid)
    {
    	pthread_create(&recv_thread, &attr, recv_secret_message, &toid);
    	while (returnnumber)
    	{	
        	returnnumber = send_secret_message(toid);//手动结束输入时退出循环
    	}
    	pthread_cancel(recv_thread);//通知子线程到达取消点取消
    	usleep(1);
    	pthread_join(recv_thread, NULL);
    }
    ```
    子线程只负责接受消息并设置已读
    ```c
    void *recv_secret_message(void *toid){
    while (1)
    {
        pthread_testcancel();//设置取消点
        sprintf(p,
                "select requests.from,requests.add_time,requests.value from requests,relationship  "
                "where requests.to= %d and requests.how=%d and requests.from=%d and "
                "requests.from=relationship.id_2 and "
                "requests.to=relationship.id_1  and relationship.if_shield=0 and "
                "requests.if_read=0;",
                userid, MESSAGES, toid);  //未屏蔽,未读的消息
        ms = cli_creatinfo(userid, 0, sql, SHOW_MESSAGES, p);
        //.........
    }
    ```
    
12.  群聊消息因为已读标志不易设置  ,不设置子线程,采用这样方式聊天
    ```c
    while (returnnumber)
    {
        recv_public_message(toid);                 //
        returnnumber = send_secret_message(toid);  // ok
    }
    ```
    
13. 文件
    发文件

    ```c
    检测权限和获取文件大小
    
    锁rs
    通知服务器(文件名,大小,接受对象)
    sendfile
    接收 服务器处理状态(成功/失败)
    解锁rs 
    ```

    收文件

    ```c
    发送文件名(sql  select like)
    获取大小,完整文件名  
        
    锁rs
    通知服务器(文件名)   输入保存路径
    recvfile
    解锁rs
        
    设置已读
    ```
14. 
    
    
    
    
    



## 遗憾&待改进

1. 未能实现 smtp的邮件重置密码功能

2. 未实现群聊消息的最新显示,每次显示定期一段时间的消息,,准备做一个时间比较显示

3.  未实现断点续传

    

# 使用

**仅在 linux 下测试，不保证全平台通用**

### 1.安装

需要安装库

    cjson
    readline
    zlog

和 

```
mariadb 或者 mysql
```

### 2.build

在项目根目录中 创建build文件夹  并在build文件夹里面执行

```c
cmake ..
make
```



### 3.启动

```
./server.out

//----------------------------

./client.out 127.0.0.1 5000
```

