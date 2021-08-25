#include "config.h"
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
extern MYSQL *sql_l;
extern char fd_name[MAXCLIENT][30];
extern int fd_id[MAXCLIENT];
file_ready f_r[MAXCLIENT];
file_ready f_s[MAXCLIENT];

long int event_recv_file_ready(events *ev)
{
    info *ms = &ev->js;
    long int f_size = 0;
    char uid[40] = {0};  //获取uuid
    {
        int ufd;
        ufd = open("/proc/sys/kernel/random/uuid", O_RDONLY);
        if (ufd < 0)
        {
            zlog_error(ser, "/proc/sys/kernel/random/uuid can't open");
            return -1;
        }
        read(ufd, uid, 39);
        if (uid[strlen(uid) - 1] == '\n' || uid[strlen(uid) - 1] == ' ') uid[strlen(uid) - 1] = 0;
        if (strlen(uid) < 30) return -2;
    }  // zlog_info(ser, "uid:%s", uid);
    char filename[100] = {0};
    int toid;
    sscanf(ms->value, "%s %ld %d", filename, &f_size, &toid);
    memset(ms->value, 0, BUFLEN);
    // zlog_info(ser, "fille:%s f_size %ld toid %d", filename, f_size,
    // toid);
    //加入数据库
    sprintf(ms->value,
            "insert into requests "
            "(requests.from,requests.to,requests.type,requests.how,requests."
            "value,requests.if_read)values(%d,%d,%d,%ld, \'%s/%s/%s\',0)",
            ms->from, toid, file, f_size, getenv("HOME"), uid,
            filename);  // uuid(36)+path
    //加入全局结构,准备接收
    {
        int i;
        for (i = 0; i < MAXCLIENT; i++)
        {
            if (f_r[i].from_id)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        if (i == MAXCLIENT)
        {
            zlog_error(ser, "f_r full");
            return -3;
        }
        f_r[i].from_id = ms->from;
        sprintf(f_r[i].path, "%s/%s/%s", getenv("HOME"), uid, filename);
        f_r[i].f_size = f_size;
        f_r[i].offset = 0;
    }
    return f_size;
}

void IN_recvfile(int cfd, int event, void *args)
{
    events *ev = (events *)args;
    epoll_del(ev);

    int i;
    {
        for (i = 0; i < MAXCLIENT; i++)
        {
            if (f_r[i].from_id == fd_id[ev->fd]) break;
        }
        if (i == MAXCLIENT)
        {
            zlog_error(ser, "未知错误");
            epoll_add(EPOLLRDHUP, ev);
            return;
        }
    }
    // zlog_info(ser, "找到 struct 信息");
    //打开文件准备存储
    if (!recv_file(ev->fd, f_r[i].path, f_r[i].f_size))
    {
        zlog_error(ser, "ser recv file error: %s", f_r[i].path);
        sprintf(ev->js.value, "%d %s[%ld] error save", 0, f_r[i].path, f_r[i].f_size);
    }
    else
    {
        // zlog_info(ser, "ser recv file success: %s", f_r[i].path);
        sprintf(ev->js.value, "%d %s[%ld] success save", 1, f_r[i].path, f_r[i].f_size);
    }

    {
        f_r[i].from_id = 0;
        //设置回复消息
        ev->js.from = 0;
        ev->js.to = fd_id[ev->fd];
        ev->js.how = IF_DONE;
        ev->js.type = msg;
        ev->call_back = justwrite;
    }

    // 展示发送内容
    char *p;
    p = showevents(ev);
    zlog_debug(ser, "recv file success ;told to:%s", p);
    free(p);

    epoll_add(EPOLLOUT, ev);
    return;
}

void OUT_sendfile(int cfd, int event, void *args)
{
    //可向对方写文件
    events *ev = (events *)args;
    info *ms = &ev->js;
    epoll_del(ev);
    // sendfile
    char *b = strchr(ms->value, '\n');
    if (b == NULL)
    {
        zlog_error(ser, "value:%s error", ms->value);
        // ev->call_back = client_event;
        epoll_add(EPOLLRDHUP, ev);
        return;
    }

    long f_size;
    char path[PATH_MAX] = {0};
    sscanf(++b, "%ld %s", &f_size, path);
    zlog_debug(ser, "ready send %ld:%s", f_size, path);

    if (!send_file(ev->fd, path, f_size))
    {
        zlog_error(ser, "path:%s can't read ", path);
        // ev->call_back = client_event;
        epoll_add(EPOLLRDHUP, ev);
        return;
    }
    ev->call_back = client_event;
    epoll_add(EPOLLIN, ev);
}

bool event_AGREE_RECV_FILE(info *ms)
{
    //没找到要接收的文件,或者找到文件太多
    if (atoi(ms->value) != 1)
    {
        // strcpy(ms->value, "0");
        //使用justwrite-cli_event普通传输
        ms->how = MANY_RESULT;
    }
    ms->to = ms->from;
    ms->from = 0;
    return true;
}
