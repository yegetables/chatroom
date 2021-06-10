#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
// #include "../include/sdebug.info.h"

void perr_exit(const char *s)
{
    perror(s);
    exit(-1);
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int n;

again:
    if ((n = accept(fd, sa, salenptr)) < 0)
    {
        if ((errno == ECONNABORTED) || (errno == EINTR))
            goto again;
        else
            perr_exit("accept error");
    }
    return n;
}

int Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    int n;

    if ((n = bind(fd, sa, salen)) < 0) perr_exit("bind error");

    return n;
}

int Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
    int n;

    if ((n = connect(fd, sa, salen)) < 0) perr_exit("connect error");

    return n;
}

int Listen(int fd, int backlog)
{
    int n;

    if ((n = listen(fd, backlog)) < 0) perr_exit("listen error");

    return n;
}

int Socket(int family, int type, int protocol)
{
    int n;

    if ((n = socket(family, type, protocol)) < 0) perr_exit("socket error");

    return n;
}

ssize_t Read(int fd, void *ptr, size_t nbytes)
{
    ssize_t n;

again:
    if ((n = read(fd, ptr, nbytes)) == -1)
    {
        if (errno == EINTR)
            goto again;
        else
            return -1;
    }
    return n;
}

ssize_t Write(int fd, const void *ptr, size_t nbytes)
{
    ssize_t n;

again:
    if ((n = write(fd, ptr, nbytes)) == -1)
    {
        if (errno == EINTR)
            goto again;
        else
            return -1;
    }
    return n;
}

int Close(int fd)
{
    int n;
    if ((n = close(fd)) == -1) perr_exit("close error");

    return n;
}

/*参三: 应该读取的字节数*/
ssize_t Readn(int fd, void *vptr, size_t n)
{
    size_t nleft;   // usigned int 剩余未读取的字节数
    ssize_t nread;  // int 实际读到的字节数
    char *ptr;

    ptr   = vptr;
    nleft = n;

    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        }
        else if (nread == 0)
            break;

        nleft -= nread;
        ptr += nread;
    }
    return n - nleft;
}

ssize_t Writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr   = vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
                return -1;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

static ssize_t my_read(int fd, char *ptr)
{
    static int read_cnt;
    static char *read_ptr;
    static char read_buf[100];

    if (read_cnt <= 0)
    {
    again:
        if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0)
        {
            if (errno == EINTR) goto again;
            return -1;
        }
        else if (read_cnt == 0)
            return 0;
        read_ptr = read_buf;
    }
    read_cnt--;
    *ptr = *read_ptr++;

    return 1;
}

ssize_t Readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++)
    {
        if ((rc = my_read(fd, &c)) == 1)
        {
            *ptr++ = c;
            if (c == '\n') break;
        }
        else if (rc == 0)
        {
            *ptr = 0;
            return n - 1;
        }
        else
            return -1;
    }
    *ptr = 0;

    return n;
}

char *itoa(int num, char *str, int radix)
{ /*索引表*/
    char index[] = "0123456789ABCDEF";
    unsigned unum; /*中间变量*/
    int i = 0, j, k;
    /*确定unum的值*/
    if (radix == 10 && num < 0) /*十进制负数*/
    {
        unum     = (unsigned)-num;
        str[i++] = '-';
    }
    else
        unum = (unsigned)num; /*其他情况*/
    /*转换*/
    do
    {
        str[i++] = index[unum % (unsigned)radix];
        unum /= radix;
    } while (unum);
    str[i] = '\0';
    /*逆序*/
    if (str[0] == '-')
        k = 1; /*十进制负数*/
    else
        k = 0;

    for (j = k; j <= (i - 1) / 2; j++)
    {
        char temp;
        temp               = str[j];
        str[j]             = str[i - 1 + k - j];
        str[i - 1 + k - j] = temp;
    }
    return str;
}

bool IsValidEmail(const char *s)
{
    char *ms;
    if ((ms = strchr(s, '@')) == NULL)
    {
        return false;
    }
    if (strchr(ms + 1, '@') != NULL)
    {
        return false;
    }
    if (strchr(ms + 1, '.') == NULL)
    {
        return false;
    }
    if (strchr(s, '.') < ms)
    {
        if (strchr(strchr(s, '.') + 1, '.') < ms)
        {
            return false;
        }
    }
    if (strlen(st rrchr(s, '.') + 1) > 4 || strlen(strrchr(s, '.') + 1) < 2)
    {
        return false;
    }
    return true;
}