#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
extern char username[30];

void cli_show_groups(void)
{
    	char p[BUFLEN] = { 0 };
	int status; //在线
	int shield; // 屏蔽
	int number = 0; //好友数量
	int online = 0;
	int j;
	info *ms = (info *)malloc(sizeof(info));
	for (j = 0; j < 4; j++) {
		memset(p, 0, sizeof(p));
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

			{
				sprintf(p,
					"select user.user_id,user.user_name from "
					"relationship,user where "
					"relationship.id_1= '%d' and "
					"user.user_id=relationship.id_2 and "
					"user.user_status= '%d' and relationship.if_shield= '%d' ;",
					userid, status, shield);
			}
		}

		{
			strcpy(ms->value, p);
			ms->type = sql;
			ms->from = userid;
			ms->to = 0;
		}
		ms = cli_send_recv(ms, FR_LIST); // base_GET_MANY_VALUE(ms, 2)
		if (ms == NULL)
			break;
		char *buf = ms->value;
		if (j == 0) {
			printf("---------好友列表-----------\n");
			printf("yourname:%s  yourid:%d\n", username, userid);
		}
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
		return;
	}
	printf("sum:%d friends,%d online\n", number, online); //在线未屏蔽人数
	printf("------------------------\n");
	if (ms)
		free(ms);
}

void show_management_groups_menu(void)
{
}