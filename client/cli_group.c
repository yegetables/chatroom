#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
extern char username[30];
extern int show_line;
void cli_show_groups(void)
{
	char p[BUFLEN] = { 0 };
	int status; //在线
	int shield; // 屏蔽
	int number = 0; //好友数量
	int online = 0;
	int j;
	printf("---------好友列表-----------\n");
	printf("yourname:%s  yourid:%d\n", username, userid);
	show_line += 2;
	info *ms = NULL;
	for (j = 0; j < 4; j++) {
		memset(p, 0, BUFLEN);
		//更改状态
		{ // 0->在线未屏蔽 10
			// 1->离线未屏蔽 00
			// 2->在线屏蔽 11
			// 3->离线屏蔽 01
			if (j == 0) {
				status = 1;
				shield = 0;
			} else if (j == 1) {
				status = 0;
				shield = 0;
			} else if (j == 2) {
				status = 1;
				shield = 1;
			} else if (j == 3) {
				status = 0;
				shield = 1;
			}
			sprintf(p,
				"select user.user_id,user.user_name from "
				"relationship,user where "
				"relationship.id_1= '%d' and "
				"user.user_id=relationship.id_2 and "
				"user.user_status= '%d' and relationship.if_shield= '%d' ;",
				userid, status, shield);

			ms = cli_creatinfo(userid, 0, sql, FR_LIST, p);

			if (ms == NULL)
				break;
		}
		char *buf = ms->value;
		// show //%d\n%d %s\n%d %s\n
		int num;
		sscanf(buf, "%d", &num);
		buf = strchr(buf, '\n');
		memset(p, 0, BUFLEN); // name
		for (int i = 0; i < num && ++buf != NULL; i++) //本次个数
		{
			int id;
			sscanf(buf, "%d %s", &id, p);
			number++;
			if (j == 0)
				online++;
			buf = strchr(buf, '\n');
			printf("%2d-->%15s (%d)", number, p, id);
			if (j == 0)
				printf("----(online)       \n"); //在线 未屏蔽
			else if (j == 1)
				printf("                   \n"); //离线 未屏蔽
			else if (j == 2)
				printf("----(online|shield)\n"); //在线 屏蔽
			else
				printf("----       (shield)\n"); //离线 屏蔽
		}
	}

	if (j != 4) {
		zlog_error(cli, "can't find all j:%d", j);
		if (ms)
			free(ms);
		printf("任意键退出\n");
		getchar();
		getchar();
		show_line += 2;
		return;
	}

	printf("sum:%d friends,%d online\n", number,
	       online); //在线未屏蔽人数
	printf("------------------------\n");
	if (ms)
		free(ms);
	printf("任意键退出\n");
	getchar();
	getchar();
	show_line += 4 + number;
	return;
}

int cli_create_group(int uid)
{
	char groupname[20] = { 0 };
	int groupid = -1;
	printf("输入群聊名称\n");
	scanf("%s", groupname);

	char p[BUFLEN] = { 0 };
	sprintf(p, "aa");
	info *ms = cli_creatinfo(userid, 0, sql, WHAT_FIRST_VALUE, p);
	if (ms == NULL) {
		zlog_error(cli, "recv none ");
		return groupid;
	}
	groupid = atoi(ms->value);

	if (ms)
		free(ms);
	return groupid;
}
