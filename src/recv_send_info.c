#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
extern zlog_category_t* ser;
#else
#include PROJECT_CLIENTHEAD
extern zlog_category_t* cli;
#endif

bool recv_info(int cfd, info* ms)
{
    zlog_category_t* tmp = NULL;
#ifdef PROJECT_SERVER
    tmp = ser;
#else
    tmp = cli;
#endif

    memset(ms, 0, sizeof(info));
    // 记录返回值
    int returnnumber = 0;
    // 记录错误次数
    int errornumber = 0;
rerecv:;
    if ((returnnumber = recv(cfd, ms, sizeof(info), 0)) != sizeof(info))
    {
        if (returnnumber == 0) return false;

        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            // 服务端不能卡死,客户端close之后服务端收到EAGAIN,三次失败直接close
            // 客户端适时等待能提高准确率,永远重试
#ifdef PROJECT_CLIENT
            sleep(1);  // magic number
            goto rerecv;
#endif
        }
        zlog_warn(tmp, "recv failed %s", show_errno());
        errornumber++;
        if (errornumber > 3)
        {
            zlog_warn(tmp, "can't recv info over 3");
            goto over;
        }
        goto rerecv;

    over:;
        zlog_error(tmp, "recv info failed %s:%s over", show_errno(), strerror(errno));
        return false;
    }
    return true;
}

bool send_info(int cfd, info* ms)
{
    zlog_category_t* tmp = NULL;
#ifdef PROJECT_SERVER
    tmp = ser;
#else
    tmp = cli;
#endif
    // 记录返回值
    int returnnumber = 0;
    // 记录错误次数
    int errornumber = 0;

resend:;
    if (sizeof(info) != (returnnumber = send(cfd, ms, sizeof(info), 0)))
    {
        if (returnnumber == 0) return false;
        zlog_warn(tmp, "send failed %s", show_errno());
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
#ifdef PROJECT_CLIENT
            sleep(1);  // magic number
            goto resend;
#endif
        }
        errornumber++;
        if (errornumber > 3)
        {
            zlog_warn(tmp, "can't send info over 3");
            goto over;
        }
        goto resend;

    over:;
        zlog_error(tmp, "send info failed %s:%s over", show_errno(), strerror(errno));
        return false;
    }
    return true;
}

bool recv_file(int cfd, char* path, long int f_size)
{
    zlog_category_t* tmp = NULL;
#ifdef PROJECT_SERVER
    tmp = ser;
#else
    tmp = cli;
#endif
    // 记录返回值
    int returnnumber = 0;
    // 记录错误次数
    int errornumber = 0;
    // char cmd[100]   = {0};
    // char dir[100]   = {0};
    // strcpy(dir, path);
    // sprintf(cmd, "mkdir -p %s", dirname(dir));
    // signal(SIGCHLD, SIG_DFL);
    // if (system(cmd) && errno != EISDIR)  //避免多次系统调用开销大
    // {
    //     signal(SIGCHLD, SIG_IGN);
    //     zlog_error(tmp, "%s error:%s ", cmd, show_errno());
    //     return false;
    // }
    // signal(SIGCHLD, SIG_IGN);

    int fd = open(path, O_RDWR | O_APPEND | O_CREAT, 0777);
    if (fd < 0)
    {
        zlog_error(tmp, "%s can't creat file %s  ", path, show_errno());
        return false;
    }
    ftruncate(fd, f_size);

    char* buf = (char*)mmap(NULL, f_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf == NULL)
    {
        zlog_error(tmp, "mmap error %s", show_errno());
        return false;
    }
    // off_t offset = 0;
resend:;
    // if (f_size != offset + (returnnumber = recv(cfd, buf, f_size -
    // offset, 0)))
    if (f_size != (returnnumber = recv(cfd, buf, f_size, 0)))
    {
        if (returnnumber == 0) return false;
        if (returnnumber < 0)
        {
            zlog_warn(tmp, "recv file failed %s", show_errno());

            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
#ifdef PROJECT_CLIENT
                sleep(1);  // magic number
                goto resend;
#endif
            }

            errornumber++;
            if (errornumber > 3)
            {
                zlog_warn(tmp, "can't recv file over 3");
                goto over;
            }
            goto resend;

        over:;
            zlog_error(tmp, "recv file failed %s:%s over", show_errno(), strerror(errno));
            return false;
        }
        else
        {
            // offset += returnnumber;
            // zlog_info(tmp, "已接收%lf", offset * 1.0 / f_size);
            goto resend;
        }
    }

    close(fd);
    munmap(buf, f_size);
    msync(buf, f_size, MS_ASYNC);
    return true;
}

bool send_file(int cfd, char* path, long int f_size)
{
    zlog_category_t* tmp = NULL;
#ifdef PROJECT_SERVER
    tmp = ser;
#else
    tmp = cli;
#endif
    // 记录返回值
    int returnnumber = 0;
    // 记录错误次数
    int errornumber = 0;

    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        zlog_error(tmp, "open %s error", path);
        return false;
    }

// off_t offset = 0;
resend:;
    /*
    ：sendfile——在文件描述符之间传输数据描述ssize_t sendfile(int out_fd, int
    in_fd, off_t *offset, size_t
    count);sendfile()在一个文件描述符和另一个文件描述符之间复制数据。因为这种复制是在内核中完成的，所以sendfile()比read(2)和write(2)的组合更高效，后者需要在用户空间之间来回传输数据。in_fd应该是打开用于读取的文件描述符，而out_fd应该是打开用于写入的文件描述符。如果offset不为NULL，则它指向一个保存文件偏移量的变量，sendfile()将从这个变量开始从in_fd读取数据。当sendfile()返回时，这个变量将被设置为最后一个被读取字节后面的字节的偏移量。如果offset不为NULL，则sendfile()不会修改当前值租用文件偏移in_fd;否则，将调整当前文件偏移量以反映从in_fd读取的字节数。如果offset为NULL，则从当前文件偏移量开始从in_fd读取数据，并通过调用更新文件偏移量。count是要在文件描述符之间复制的字节数。in_fd参数必须对应于支持类似mmap(2)的操作的文件(也就是说，它不能是套接字)。在2.6.33之前的Linux内核中，out_fd必须引用一个套接字。从Linux
    2.6.33开始，它可以是任何文件。如果是一个常规文件，则sendfile()适当地更改文件偏移量。
    */
    // TODO(ajian): 断点续传
    // fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) &~ O_NONBLOCK);
    // if (offset + (returnnumber = sendfile(cfd, fd, &offset, f_size) !=
    // f_size))
    if ((returnnumber = sendfile(cfd, fd, NULL, f_size)) != f_size)
    {
        // zlog_error(tmp, "returnnumber %d offset %ld", returnnumber,
        // offset);
        if (returnnumber == 0)
        {
            zlog_error(tmp, "return 0");
            return false;
        }
        if (returnnumber < 0)
        {
            zlog_warn(tmp, "send file failed %s", show_errno());
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
#ifdef PROJECT_CLIENT
                sleep(1);  // magic number
                goto resend;
#endif
            }
            errornumber++;
            if (errornumber > 3)
            {
                zlog_warn(tmp, "can't send file over 3");
                goto over;
            }
            goto resend;

        over:;
            zlog_error(tmp, "send file failed %s:%s over", show_errno(), strerror(errno));
            return false;
        }
        else if (returnnumber > 0)
        {
            // offset += returnnumber;
            // zlog_info(tmp, "已发送%lf", offset * 1.0 / f_size);
            goto resend;
        }
    }

    // zlog_info(tmp, "offset=%ld", offset);
    return true;
}
