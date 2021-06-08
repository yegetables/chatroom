#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
#else
#include PROJECT_CLIENTHEAD
#endif
char* show_signal(int signal)
{
#ifdef SIGHUP
    if (signal == SIGHUP) return "SIGHUP";  // 1终端的挂断或进程死亡
#endif
#ifdef SIGINT
    if (signal == SIGINT) return "SIGINT";  // 2来自键盘的中断信号
#endif
#ifdef SIGQUIT
    if (signal == SIGQUIT) return "SIGQUIT";  // 3来自键盘的离开信号
#endif
#ifdef SIGILL
    if (signal == SIGILL) return "SIGILL";  // 4非法指令
#endif
#ifdef SIGABRT
    if (signal == SIGABRT) return "SIGABRT";  // 6来自abort的异常信号
#endif
#ifdef SIGFPE
    if (signal == SIGFPE) return "SIGFPE";  // 8浮点例外
#endif
#ifdef SIGKILL
    if (signal == SIGKILL) return "SIGKILL";  // 9杀死
#endif
#ifdef SIGSEGV
    if (signal == SIGSEGV) return "SIGSEGV";  // 11段非法错误(内存引用无效)
#endif
#ifdef SIGPIPE
    if (signal == SIGPIPE) return "SIGPIPE";  // 13管道损坏/向没有读的管道写数据
#endif
#ifdef SIGALRM
    if (signal == SIGALRM) return "SIGALRM";  // 14来自alarm的计时器到时信号
#endif
#ifdef SIGTERM
    if (signal == SIGTERM) return "SIGTERM";  // 15终止
#endif
#ifdef SIGUSR1
    if (signal == SIGUSR1) return "SIGUSR1";  // 用户自定义信号1
#endif
#ifdef SIGUSR2
    if (signal == SIGUSR2) return "SIGUSR2";  // 用户自定义信号2
#endif
#ifdef SIGCHLD
    if (signal == SIGCHLD) return "SIGCHLD";  // 子进程停止或终止
#endif
#ifdef SIGCONT
    if (signal == SIGCONT) return "SIGCONT";  // 如果停止，继续执行
#endif
#ifdef SIGSTOP
    if (signal == SIGSTOP) return "SIGSTOP";  // 非来自终端的停止信号
#endif
#ifdef SIGTSTP
    if (signal == SIGTSTP) return "SIGTSTP";  // 来自终端的停止信号
#endif
#ifdef SIGTTIN
    if (signal == SIGTTIN) return "SIGTTIN";  // 后台进程读终端
#endif
#ifdef SIGTTOU
    if (signal == SIGTTOU) return "SIGTTOU";  // 后台进程写终端
#endif
#ifdef SIGBUS
    if (signal == SIGBUS) return "SIGBUS";  // 总线错误（内存访问错误）
#endif
#ifdef SIGPOLL
    if (signal == SIGPOLL) return "SIGPOLL";  // SIGIO同义
#endif
#ifdef SIGPROF
    if (signal == SIGPROF) return "SIGPROF";  // 统计分布图用计时器到时
#endif
#ifdef SIGSYS
    if (signal == SIGSYS) return "SIGSYS";  // 非法系统调用 (SVr4)
#endif
#ifdef SIGTRAP
    if (signal == SIGTRAP) return "SIGTRAP";  // 跟踪 / 断点自陷
#endif
#ifdef SIGURG
    if (signal == SIGURG) return "SIGURG";  // socket 紧急信号 (4.2BSD)
#endif
#ifdef SIGVTALRM
    if (signal == SIGVTALRM) return "SIGVTALRM";  // 虚拟计时器到时 (4.2BSD
#endif
#ifdef SIGXCPU
    if (signal == SIGXCPU) return "SIGXCPU";  // 超过 CPU 时限 (4.2BSD)
#endif
#ifdef SIGXFSZ
    if (signal == SIGXFSZ) return "SIGXFSZ";  // 超过文件长度限制 (4.2BSD)
#endif
#ifdef SIGIOT
    if (signal == SIGIOT) return "SIGIOT";  // IOT 自陷，与 SIGABRT 同义
#endif
#ifdef SIGEMT
    if (signal == SIGEMT) return "SIGEMT";  // 	Term
#endif
#ifdef SIGSTKFLT
    if (signal == SIGSTKFLT) return "SIGSTKFLT";  // 协处理器堆栈错误 (不使用)
#endif
#ifdef SIGIO
    if (signal == SIGIO) return "SIGIO";  //描述符上可以进行 I/O 操作
#endif
#ifdef SIGCLD
    if (signal == SIGCLD) return "SIGCLD";  // 与 SIGCHLD 同义 #endif
#endif
#ifdef SIGPWR
    if (signal == SIGPWR) return "SIGPWR";  // 电力故障 (System V)
#endif
#ifdef SIGINFO
    if (signal == SIGINFO) return "SIGINFO";  // 	与 SIGPWR 同义
#endif
#ifdef SIGLOST
    if (signal == SIGLOST) return "SIGLOST";  // 文件锁丢失
#endif
#ifdef SIGWINCH
    if (signal == SIGWINCH) return "SIGWINCH";  // 窗口大小改变 (4.3BSD, Sun)
#endif
#ifdef SIGUNUSED
    if (signal == SIGUNUSED) return "SIGUNUSED";  // 未使用信号 (will be SIGSYS)
#endif
    return NULL;
}