#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
extern char username[30];
extern int cfd;
extern int show_line;
extern pthread_mutex_t rs_mutex;
void cli_recv_file(int toid, int is)
{
    char *fname = NULL;  //文件模糊名
    info *ms = NULL;
    char *sname = NULL;      //本地保存文件路径
    char relname[PATH_MAX];  //服务器真实文件名
    char *b;
    char p[BUFLEN] = {0};
resdd:;
    // memset(fname, 0, sizeof(fname));
    // printf("输入文件名\n");
    // scanf("%s", fname);
    // unsigned long long len = PATH_MAX;
    // getline(fname, &len, stdin);
    fname = readline("输入文件名:");
    show_line++;

    // char realpath[PATH_MAX] = {0};
    // char p[BUFLEN] = {0};
    // memset(p, 0, BUFLEN);
    sprintf(p,
            "select requests.how,requests.value  from requests  where "
            "requests.to= %d and requests.from=%d and requests.if_read=0 and requests.type=%d"
            " and requests.value like \'%%%s\' ",
            userid, toid, file, fname);  //模糊搜索
    // base_GET_MANY_VALUE(ms, 2)
    ms = cli_creatinfo(userid, 0, sql, GET_MANY_VALUE, p);  //得到大小,确认数量
    if (ms == NULL)
    {
        if (fname) free(fname);
        zlog_error(cli, "recv none");
        return;
    }

    if (1 != atoi(ms->value))
    {
        if (ms) free(ms);
        if (fname) free(fname);
        printf("结果太多或不存在,重新搜索\n");
        show_line++;
        goto resdd;
    }

    if (is == 1)  //同意
    {
        long f_size;
        ms->value[0] = ' ';
        ms->value[1] = ' ';
        sscanf(ms->value, "%ld %s", &f_size, relname);  //得到大小,确认数量
        free(ms);
        sname = readline("输入保存到文件的绝对路径:");
        show_line++;
        //通知服务器 s->c send file

        // b = strchr(ms->value, '\n');
        // b++;
        // memset(sname, 0, PATH_MAX);
        // printf("输入保存到文件的绝对路径\n");
        // scanf("%s", sname);

        // char* f = dirname(fname);             //目录
        // sprintf(realpath, "%s/%s", f, name);  //拼接目录/文件

        // zlog_error(cli, "save %s to path %s", relname, sname);

        //打开文件准备存储
        // pthread_mutex_lock(&rs_mutex);
        // ms = cli_send_recv(ms, how);

        //不接受回复和不解锁
        ms = cli_creatinfo(userid, 0, sql, AGREE_RECV_FILE, p);
        if (!recv_file(cfd, sname, f_size))
        {
            pthread_mutex_unlock(&rs_mutex);
            zlog_error(cli, "recv file error: %s", sname);
            if (ms) free(ms);
            if (fname) free(fname);
            if (sname) free(sname);
            return;
        }
        pthread_mutex_unlock(&rs_mutex);
        zlog_info(cli, "recv file success: %s", sname);
    }
    // fname 客户接受路径

    //设置已读
    memset(p, 0, sizeof(p));
    sprintf(p,
            "update requests,relationship set requests.if_read=1 "
            "where "
            "requests.to= %d   and  requests.value like \'%%%s\' and  "
            "requests.type=%d and "
            "requests.from= %d",
            userid, fname, file, toid);  //设为已读
    // base_GET_MANY_VALUE(ms, 2)
    ms = cli_creatinfo(userid, 0, sql, IF_DONE, p);

    if (ms == NULL)
    {
        zlog_error(cli, "set file had read error");
        // if (ms) free(ms);
        if (fname) free(fname);
        if (sname) free(sname);
        return;
    }
    if (is == 1)
    {
        zlog_info(cli, "recv file %s to %s del from %d to %d ", relname, sname, toid, userid);
        printf("已接受文件%s to %s", relname, sname);
    }
    else
    {
        zlog_info(cli, " del file from %d to %d ", toid, userid);
        printf("已拒绝文件%s", fname);
    }
    PAUSE;
    // show_line++;
    if (ms) free(ms);
    if (fname) free(fname);
    if (sname) free(sname);
    return;
}

void show_apply_files(int toid)
{
    char p[BUFLEN] = {0};
    memset(p, 0, BUFLEN);

    info *ms = NULL;
    sprintf(p,
            "select requests.value   from requests,relationship  "
            "where requests.to= "
            "\'%d\' and "
            "requests.type=\'%d\' and requests.from=\'%d\' and "
            "requests.from=relationship.id_2 and "
            "requests.to=relationship.id_1  and relationship.if_shield=0 "
            "and requests.if_read=0 ;",
            userid, file, toid);  //未屏蔽的文件

    ms = cli_creatinfo(userid, 0, sql, GET_MESSAGE_FROM, p);
    // base_GET_MANY_VALUE(ms, 1)
    if (ms == NULL)
    {
        zlog_error(cli, "recv none");
        return;
    }
    char *buf = ms->value;
    // printf("%s\n", buf);
    int num = 0;
    sscanf(buf, "%d", &num);
    buf = strchr(buf, '\n');                         // name
    for (int i = 1; i <= num && ++buf != NULL; i++)  //本次个数
    {
        memset(p, 0, sizeof(p));
        sscanf(buf, "%s", p);
        buf = strchr(buf, '\n');
        printf("%2d-->%15s \n", i, p);
        // printf("%2d-->%15s \n", i, basename(p));
    }
    show_line += num + 1;
    printf("----------sum:%d--------\n", num);
    if (ms) free(ms);
    return;
}

void send_file_menu(int toid)
{
    // cat /proc/sys/kernel/random/uuid 随机uuid
    // printf("输入文件路径\n");
    // char path[PATH_MAX] = {0};
    // scanf("%s", path);
    char *path = NULL;
    path = readline("输入文件路径:");
    show_line++;
    // //检测文件名长度
    // if (strlen(path) > )
    // {
    //     printf("file name too long \n");
    //     show_line++;
    //     free(path);
    //     zlog_error(cli, "file name too long ");
    //     return;
    // }

    // while (path[strlen(path) - 1] == '\n' || path[strlen(path) - 1] == '\r' ||
    //        path[strlen(path) - 1] == ' ')
    // {
    //     path[strlen(path) - 1] = '\0';
    // }
    // path[strlen(path) - 1] = '\n';

    // 内部检测权限
    long int f_size = get_file_size(path);
    if (f_size < 0)
    {
        zlog_error(cli, "get size error");
        if (path)
        {
            free(path);
            path = NULL;
        }
        printf("发送失败");
        PAUSE;
        return;
    }

    char *filename = basename(path);
    printf("waiting transmission\n");
    show_line++;

    //发送文件通知
    info *ms = NULL;
    char p[BUFLEN] = {0};
    sprintf(p, "%s %ld %d", filename, f_size, toid);

    zlog_info(cli, "told ser ready send file[SEND_FILE_REDY] :%s", p);
    ms = cli_creatinfo(userid, 0, sql, SEND_FILE_REDY, p);  //锁上
    if (ms == NULL || atoi(ms->value) != 1)
    {
        pthread_mutex_unlock(&rs_mutex);
        zlog_info(cli, "ser refused recv file %s  ", path);
        printf("服务器未准备好接收文件\n");
        show_line++;
        if (ms)
        {
            free(ms);
            ms = NULL;
        }
        if (path)
        {
            free(path);
            path = NULL;
        }
        PAUSE;
        return;
    }
    zlog_debug(cli, "ser recv file agree;;cli real sendfile : %s", path);

    //真正发送文件
    if (!send_file(cfd, path, f_size))
    {
        zlog_error(cli, "real cli sendfile ser error:path:%s  ", path);
        pthread_mutex_unlock(&rs_mutex);
        if (ms)
        {
            free(ms);
            ms = NULL;
        }
        if (path)
        {
            free(path);
            path = NULL;
        }
        printf("发送失败");
        PAUSE;
        return;
    }

    zlog_info(cli, "real send to ser success ");

    // pthread_mutex_unlock(&rs_mutex);

    // 接受并展示info
    if (recv_info(cfd, ms))
    {
        zlog_info(cli, "recv file info");
        if (atoi(ms->value))
        {
            printf("发送成功");
            zlog_info(cli, " recv file message success");
        }
    }
    else
    {
        zlog_info(cli, "can;t recv file info");
        printf("发送失败");
    }

    pthread_mutex_unlock(&rs_mutex);

    if (ms)
    {
        free(ms);
        ms = NULL;
    }
    if (path)
    {
        free(path);
        path = NULL;
    }
    PAUSE;
    // zlog_debug(cli, "recv errror");
    return;  // no close
}
