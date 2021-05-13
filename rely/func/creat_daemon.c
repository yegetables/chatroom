#include "../../include/chat.h"
int creat_daemon(void)
{
    //1.创建子进程，父进程退出
    pid_t pid = fork();
    if (pid == 0)
    {
        //2.创建新会话,使子进程完全独立出来，脱离控制
        pid = setsid();

        //再次创建子进程(第二次保证进程不是进程组长)
        pid = fork();
        if (pid == 0)
        {
            ;
        }
        else if (pid == -1)
        {
            perror("fork");
            return -1;
        }
        else
        {
            exit(0);
        }
    }
    else if (pid == -1)
    {
        perror("fork");
        return -1;
    }
    else
    {
        exit(0);
    }

    //3.切换目录
    //chdir("/");

    //4.重设文件权限掩码
    //umask(0);

    //5.关文件
    for (int i = 0; i < NOFILE; close(i++))
        ;

    //6.信号处理
    //如果对于信号处理通过怕生成子进程执行,要设置信号忽略SIGCHLD信号,可以避免子进程成为僵尸
    signal(SIGCHLD, SIG_IGN);

    //7.其他,可以处理别的
    //也可以返回主程序,执行任务,此时主程序成为守护进程
    return 0;
}