#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
extern char username[30];
extern int show_line;

void cli_show_groups(void)
{
	;
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
