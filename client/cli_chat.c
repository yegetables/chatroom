#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
extern char username[30];
extern int show_line;
int send_secret_message(int toid)
{
	info *ms = NULL;
	char p[BUFLEN / 2] = { 0 };
	char pp[BUFLEN] = { 0 };
	printf("\nto:%d:", toid);
	scanf("%s", p);
	show_line += 2;
	if (strcmp(p, "#return#") == 0)
		return 0; //返回上一层
	sprintf(pp,
		"INSERT  INTO requests "
		"(requests.from,requests.to,requests.type,requests.how,requests."
		"value,requests.if_read)"
		"VALUES (%d,%d,%d,%d,\'%s\',%d);",
		userid, toid, msg, MESSAGES, p, 0);
	ms = cli_creatinfo(userid, 0, sql, HUP_NO, pp);
	if (ms == NULL)
		return 0;
	if (ms)
		free(ms);
	return 1; //继续聊天
}

void recv_secret_message(int toid)
{
	info *ms = NULL;
	char p[BUFLEN] = { 0 };
	sprintf(p,
		"select requests.add_time,requests.value from requests,relationship  "
		"where requests.to= %d and requests.how=%d and requests.from=%d and "
		"requests.from=relationship.id_2 and "
		"requests.to=relationship.id_1  and relationship.if_shield=0 and "
		"requests.if_read=0;",
		userid, MESSAGES, toid); //未屏蔽,未读的消息
	ms = cli_creatinfo(userid, 0, sql, SHOW_MESSAGES, p);
	if (ms == NULL)
		return;
	if (atoi(ms->value) != 0) {
		char *buf = strchr(ms->value, '\n');
		buf++;
		printf("\nfrom %d :%s", toid, buf);
		show_line++;
	}
	memset(p, 0, BUFLEN);
	sprintf(p,
		"update requests,relationship set requests.if_read=1 "
		"where requests.to= %d and requests.how=%d and requests.from=%d and "
		"requests.from=relationship.id_2 and "
		"requests.to=relationship.id_1  and relationship.if_shield=0 and "
		"requests.if_read=0;",
		userid, MESSAGES, toid); //设为已读
	ms = cli_creatinfo(userid, 0, sql, HUP_NO, p);
	if (ms == NULL)
		return;
	if (ms)
		free(ms);
}

void show_secret_message(int toid)
{
	info *ms = NULL;
	char p[BUFLEN] = { 0 };
	sprintf(p,
		"select requests.add_time,requests.value from requests,relationship  "
		"where requests.to= %d and requests.how=%d and requests.from=%d and "
		"requests.from=relationship.id_2 and "
		"requests.to=relationship.id_1  ;",
		userid, MESSAGES, toid); //未屏蔽,未读的消息
	ms = cli_creatinfo(userid, 0, sql, SHOW_MESSAGES, p);
	if (ms == NULL)
		return;
	int number = atoi(ms->value);
	printf("have %d messages:\n", number);
	show_line++;
	if (number != 0) {
		char *buf = strchr(ms->value, '\n');
		buf++;
		printf("\nfrom %d :%s", toid, buf); // BUG:showline
		show_line++;
	}
	PAUSE;
	if (ms)
		free(ms);
}

void secret_message_menu(int toid)
{
	int returnnumber = 1;
	printf("随时输入(#return#)返回上一层\n");
	show_line++;
	while (returnnumber) {
		recv_secret_message(toid);
		returnnumber = send_secret_message(toid);
	}
	return;
}

void recv_public_message(int toid)
{
	info *ms = NULL;
	char p[BUFLEN] = { 0 };
	sprintf(p,
		"select requests.add_time,requests.from,requests.value from requests,relationship  "
		"where requests.to= %d and requests.how=%d  and "
		"requests.from=relationship.id_2 and "
		"requests.to=relationship.id_1  and relationship.if_shield=0 and "
		"requests.if_read=0;",
		toid, MESSAGES); //未屏蔽,未读的消息

	ms = cli_creatinfo(userid, 0, sql, SHOW_GROUP_MESSAGES, p);
	if (ms == NULL)
		return;
	if (atoi(ms->value) != 0) {
		char *buf = strchr(ms->value, '\n');
		buf++;
		printf("\nto %d:%s", toid, buf); // BUG:showline
		show_line++;
	}
	// TODO:群聊消息设置已读
	// memset(p, 0, BUFLEN);
	// sprintf(p,
	// 	"update requests,relationship set requests.if_read=1 "
	// 	// "where requests.to= %d and requests.how=%d and
	// 	// requests.from=%d and " "requests.from=relationship.id_2 and "
	// 	// "requests.to=relationship.id_1  and relationship.if_shield=0
	// 	// and " "requests.if_read=0;",
	// 	"where requests.to= %d and requests.how=%d  and "
	// 	"requests.from=relationship.id_2 and "
	// 	"requests.to=relationship.id_1  and relationship.if_shield=0 and
	// " 	"requests.if_read=0;", 	toid, MESSAGES); //设为已读 ms =
	// cli_creatinfo(userid, 0, sql, HUP_NO, p);
	if (ms == NULL)
		return;
	if (ms)
		free(ms);
}

void show_public_message(int toid)
{
	info *ms = NULL;
	char p[BUFLEN] = { 0 };
	sprintf(p,
		"select requests.add_time,requests.from,requests.value from requests,relationship  "
		"where requests.to= %d and requests.how=%d  and "
		"requests.from=relationship.id_2 and "
		"requests.to=relationship.id_1  ;",
		toid, MESSAGES); //未屏蔽,未读的消息
	ms = cli_creatinfo(userid, 0, sql, SHOW_GROUP_MESSAGES, p);
	if (ms == NULL)
		return;
	int number = atoi(ms->value);
	printf("have %d messages:\n", number);
	show_line++;
	if (number != 0) {
		char *buf = strchr(ms->value, '\n');
		buf++;

		printf("\nto %d:%s", toid, buf); // BUG:showline
		show_line++;
	}
	PAUSE;
	if (ms)
		free(ms);
}

void public_message_menu(int toid)
{
	int returnnumber = 1;
	printf("随时输入(#return#)返回上一层\n");
	show_line++;
	while (returnnumber) {
		recv_public_message(toid); //
		returnnumber = send_secret_message(toid); // ok
	}
	return;
}