#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
extern char username[30];
extern int show_line;

void cli_show_groups(void)
{
	char p[BUFLEN] = { 0 };
	int number = 0;
	sprintf(p,
		"select group_id,group_name from "
		" groups,relationship where "
		" relationship.id_1=%d and "
		" relationship.id_2=group_id  ",
		userid);
	info *ms = cli_creatinfo(userid, 0, sql, GR_LIST, p);
	if (ms == NULL) {
		zlog_error(cli, "recv error");
		return;
	}
	printf("---------群组列表-----------\n");
	printf("yourname:%s  yourid:%d\n", username, userid);
	show_line += 2;
	char *buf = ms->value;
	// show //%d\n%d %s\n%d %s\n
	int num;
	sscanf(buf, "%d", &num);
	buf = strchr(buf, '\n');

	for (int i = 0; i < num && ++buf != NULL; i++) //本次个数
	{
		memset(p, 0, BUFLEN); // name
		int id;
		sscanf(buf, "%d %s", &id, p);
		number++;
		buf = strchr(buf, '\n');
		printf("%2d-->%15s (%d)\n", number, p, id);
	}
	printf("sum:%d groups\n", number); //在线未屏蔽人数
	printf("------------------------\n");
	show_line += 2 + number;
	PAUSE;
	if (ms)
		free(ms);
	return;
}

int cli_create_groups(int uid)
{
	char groupname[20] = { 0 };
	int groupid = -1;
	printf("输入群聊名称\n");
	scanf("%s", groupname);
	char p[BUFLEN] = { 0 };
	sprintf(p,
		"insert into groups (group_name,master_id)values(\'%s\',%d);",
		groupname, userid);
	info *ms = cli_creatinfo(userid, 0, sql, CREATE_GROUP, p);
	if (ms == NULL) {
		zlog_error(cli, "recv none ");
		return groupid;
	}
	groupid = atoi(ms->value);
	printf("群组id为%d\n", groupid);
	show_line += 3;
	PAUSE;
	if (ms)
		free(ms);
	return groupid;
}

bool cli_del_groups(int userid)
{
	// char groupname[20] = { 0 };
	int groupid = -1;
	printf("输入群聊id\n");
	scanf("%d", &groupid);
	char p[BUFLEN] = { 0 };

	sprintf(p, "select * from groups where  group_id=%d  and master_id=%d;",
		groupid, userid);
	info *ms = cli_creatinfo(userid, 0, sql, DEL_GROUP, p);
	if (ms == NULL) {
		zlog_error(cli, "recv none ");
		return false;
	}
	if (atoi(ms->value) != 1) {
		printf("未找到群主为%d,群号为%d的群\n", userid, groupid);

	} else {
		printf("群组id为%d的群已解散\n", groupid);
	}
	show_line += 3;
	if (ms)
		free(ms);
	PAUSE;
	return true;
}

void cli_show_groups_members(void)
{
	int groupid = -1;
	printf("输入已加入的群组id查看群成员\n");
	scanf("%d", &groupid);
	show_line += 2;

	char p[BUFLEN] = { 0 };
	int number = 0;
	sprintf(p,
		"select user.user_id,user.user_name from "
		" groups,relationship,user where "
		// " relationship.id_1=%d and "
		// " relationship.id_2=groups.group_id and  "
		" relationship.id_1=user.user_id and "
		" relationship.id_2=%d ",
		groupid);
	info *ms = cli_creatinfo(userid, 0, sql, GR_LIST, p);
	if (ms == NULL) {
		zlog_error(cli, "recv error");
		return;
	}
	printf("---------群组成员-----------\n");
	// printf("yourname:%s  yourid:%d\n", username, userid);
	show_line += 1;
	char *buf = ms->value;
	// show //%d\n%d %s\n%d %s\n
	int num;
	sscanf(buf, "%d", &num);
	buf = strchr(buf, '\n');

	for (int i = 0; i < num && ++buf != NULL; i++) {
		memset(p, 0, BUFLEN);
		int id;
		sscanf(buf, "%d %s", &id, p);
		number++;
		buf = strchr(buf, '\n');
		printf("%2d-->%15s (%d)\n", number, p, id);
	}
	printf("sum:%d members\n", number);
	printf("------------------------\n");
	show_line += 2 + number;
	PAUSE;
	if (ms)
		free(ms);
	return;
}