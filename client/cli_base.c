#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int cfd;
extern int epfd;
extern struct epoll_event tempevents;
extern int userid;
extern int applications;
extern int messages;
extern int files;
info *cli_send_recv(info *ms, int how)
{
    if (ms == NULL)
    {
        return NULL;
    }
    ms->how = how;

    tempevents.events = EPOLLOUT;
    epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tempevents);
    struct epoll_event outevents[1024];
    while (1)
    {
        int thisnum = epoll_wait(epfd, outevents, 1024, 0);
        if (thisnum != 1) continue;
        if (outevents[0].events & EPOLLIN)
        {
            epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
            // 接受并展示info
            if (recv_info(cfd, ms))
            {
                char *tt = NULL;
                tt = showinfo(ms);
                zlog_debug(cli, "%s", tt);
                free(tt);
                return ms;
            }
            else
            {
                if (ms)
                {
                    free(ms);
                    ms = NULL;
                }
                return ms;  // no close
            }
        }
        if (outevents[0].events & EPOLLOUT)
        {
            epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
            // 发送查询sql
            if (!send_info(cfd, ms))
            {
                if (ms)
                {
                    free(ms);
                    ms = NULL;
                }
                return ms;  // no close
            }
            if (how == HUP_NO)
            {
                return ms;
            }
            tempevents.events = EPOLLIN;
            epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tempevents);
        }
    }

    // if (how != HUP_NO)
    // {
    //     // 接受并展示info
    //     if (recv_info(cfd, ms))
    //     {
    //         char *tt = NULL;
    //         tt = showinfo(ms);
    //         zlog_debug(cli, "%s", tt);
    //         free(tt);
    //         return ms;
    //     }
    //     else
    //     {
    //         if (ms)
    //         {
    //             free(ms);
    //             ms = NULL;
    //         }
    //         return ms;  // no close
    //     }
    // }
    // else
    // {
    //     return ms;
    // }
}

void update_notices(char *user_msg, char *user_files)
{
    applications = 0;
    messages = 0;
    files = 0;
    int id_num[30000] = {0};
    char p[BUFLEN] = {0};
    memset(user_msg, 0, BUFLEN);
    memset(user_files, 0, BUFLEN);
    info *ms = NULL;
    {
        memset(p, 0, BUFLEN);
        sprintf(p,
                "select * from requests  where requests.to= \'%d\' and "
                "requests.how=\'%d\';",
                userid, ADD_FRIEND);
        ms = cli_creatinfo(userid, 0, sql, MANY_RESULT, p);
        if (ms == NULL)
        {
            zlog_error(cli, "get applications failed ");
            return;
        }
    }

    applications = atoi(ms->value);
    {
        {
            memset(p, 0, sizeof(p));
            sprintf(p,
                    "select  requests.from from requests,relationship  "
                    "where requests.to= "
                    "\'%d\' and "
                    "requests.how=\'%d\' and requests.from=relationship.id_2 and "
                    "requests.to=relationship.id_1  and relationship.if_shield=0 "
                    "and requests.if_read=0 ;",
                    userid, MESSAGES);  //未屏蔽的消息
            ms = cli_creatinfo(userid, 0, sql, GET_MESSAGE_FROM, p);
            if (ms == NULL)
            {
                zlog_error(cli, "get messages failed ");
                return;
            }
        }
        messages = atoi(ms->value);
        char *b = strchr(ms->value, '\n');
        if (b == NULL)
        {
            zlog_error(cli, "value:%s", ms->value);
            exit(-1);
        }

        b++;
        int maxid = 0;
        int minid = 300000;

        for (int i = 0; i < messages && b != NULL; i++, b++)
        {
            int id;
            sscanf(b, "%9d", &id);
            if (id > maxid)
            {
                maxid = id;
            }
            if (id < minid)
            {
                minid = id;
            }
            id_num[id]++;
            b = strchr(b, '\n');
        }

        for (int j = minid; j <= maxid; j++)
        {
            if (id_num[j] > 0)
            {
                sprintf(&user_msg[strlen(user_msg)], "[%d:%d]", j, id_num[j]);
            }
        }
    }

    {
        memset(p, 0, sizeof(p));
        sprintf(p,
                "select  requests.from from requests,relationship  "
                "where requests.to= \'%d\' and requests.type=\'%d\'"
                " and requests.from=relationship.id_2 and "
                "requests.to=relationship.id_1  and relationship.if_shield=0 "
                "and requests.if_read=0 ;",
                userid, file);  //未屏蔽的文件
        ms = cli_creatinfo(userid, 0, sql, GET_MESSAGE_FROM, p);
        if (ms == NULL)
        {
            zlog_error(cli, "get messages failed ");
            return;
        }
    }

    files = atoi(ms->value);

    {
        char *b = strchr(ms->value, '\n');
        if (b == NULL)
        {
            zlog_error(cli, "value:%s", ms->value);
            exit(-1);
        }

        b++;
        int maxid = 0;
        int minid = 300000;
        memset(id_num, 0, sizeof(id_num));
        for (int i = 0; i < files && b != NULL; i++, b++)
        {
            int id;
            sscanf(b, "%9d", &id);

            if (id > maxid)
            {
                maxid = id;
            }
            if (id < minid)
            {
                minid = id;
            }
            id_num[id]++;
            b = strchr(b, '\n');
        }
        for (int j = minid; j <= maxid; j++)
        {
            if (id_num[j] > 0)
            {
                sprintf(&user_files[strlen(user_files)], "[%d:%d]", j, id_num[j]);
            }
        }
    }

    if (ms)
    {
        free(ms);
    }
}

long int get_file_size(char *path)
{
    zlog_category_t *tmp = NULL;

    tmp = cli;
    if (access(path, F_OK) || access(path, R_OK))
    {
        zlog_error(tmp, "path:%s can't read ", path);
        printf("error path\n");
        return -1;
    }
    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        zlog_error(tmp, "open %s error", path);
        return -1;
    }
    struct stat statbuf;
    memset(&statbuf, 0, sizeof(struct stat));
    if (fstat(fd, &statbuf))
    {
        zlog_error(tmp, "fstat error %s", strerror(errno));
        return -1;
    }
    return statbuf.st_size;
}

info *cli_creatinfo(int from, int to, value_type type, int how, char *value)
{
    info *ms = (info *)malloc(sizeof(info));
    strcpy(ms->value, value);
    ms->how = how;
    ms->type = type;
    ms->from = from;
    ms->to = to;
    ms = cli_send_recv(ms, how);
    return ms;
}