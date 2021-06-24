#include "config.h"
#include PROJECT_SERVERHEAD

int port = 500; // var 服务器端口
int epfd; // 全局epfd
events g_events[MAXCLIENT + 1];
zlog_category_t *ser = NULL;
MYSQL *sql_l = NULL;
// 在线列表fd->id
int fd_id[MAXCLIENT];

// 在线列表fd->name
char fd_name[MAXCLIENT][30];
int main(int argc, char **argv)
{
	/// 解析命令行
	{
		char *options = "h";
		int opt;
		while ((opt = getopt(argc, argv, options)) != -1) {
			/**
			 * @brief getopt
			 * @note optarg —— 指向当前选项参数(如果有) 的指针。
			 * @note optind —— 再次调用 getopt()时的下一个
			 * argv指针的索引。
			 * @note optopt —— 最后一个未知选项。
			 * @note opterr ­——
			 * 如果不希望getopt()打印出错信息，则只要将全域变量opterr设为0即可。
			 */

			if (opt == 'h')
				help();

			///  @todo help手册
			printf("******************************\n");
		}
	}

	/// 读配置
	ser_setconfig(PROJECT_SERCONFIG);
	printf("server port:%d\n", port);

	/// 守护进程
	creat_daemon();

	/// 开日志
	{
		{
			char *cmd = (char *)calloc(BUFLEN, sizeof(char));
			sprintf(cmd, "rm %s*server*.log", PROJECT_LOGPATH);
			system(cmd);
			sprintf(cmd, "rm %s*client*.log", PROJECT_LOGPATH);
			system(cmd);
			free(cmd);
		}

		ser = my_zlog_init("server");
		zlog_info(ser, "--------start--------");
		zlog_info(ser, "pid[%d]   port[%d]", getpid(), port);
		// zlog_debug(ser, "epfd[%d]", epfd);
	}

	/// sql 连接
	{
		sql_l = sql_connect();
		if (sql_l == NULL) {
			zlog_error(ser, "sql_connect error");
			exit(-1);
		}
		if (!sql_init_table(sql_l)) {
			exit(-1);
		}
	}

	/// @brief epoll反应堆模型
	epfd = epoll_create(MAXCLIENT);

	///  @todo 多线程
	int lfd = epoll_init_lfd();
	zlog_debug(ser, "lfd:%d", lfd);

	//仅供调用回调
	struct epoll_event tempevents[MAXCLIENT + 1];
	while (1) {
		int max = epoll_wait(epfd, tempevents, MAXCLIENT, 0);
		for (int i = 0; i < max; i++) {
			events *this = tempevents[i].data.ptr;
			if (tempevents[i].events & EPOLLRDHUP) {
				epoll_del(this);
				close(this->fd);
				// status=0;下次accpet会重置
				char *p = showevents(this);
				// zlog_debug(ser, "rdhup:%s", p);
				free(p);
				if (strlen(fd_name[this->fd])) {
					zlog_info(
						ser,
						"**leave**   fd:%d id:%d name:%s",
						this->fd, fd_id[this->fd],
						fd_name[this->fd]);
					char *cmd = (char *)calloc(
						BUFLEN, sizeof(char));
					sprintf(cmd,
						"update user set user_status=\'0\' where "
						"user_name=\'%s\';",
						fd_name[this->fd]);
					mysql_query(sql_l, cmd);
					free(cmd);
					memset(fd_name[this->fd], 0, 30);
				}
				fd_id[this->fd] = 0;

				continue;
			}
			this->call_back(this->fd, this->events, this->arg);
		}
	}

	zlog_fini();
	mysql_close(sql_l);
	return 0;
}