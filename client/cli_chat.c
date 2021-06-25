#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
extern char username[30];

int send_secret_message(int toid)
{
	info *ms = (info *)malloc(sizeof(info));
	char p[BUFLEN / 2];
	printf("\nto:%d:", toid);
	memset(p, 0, sizeof(p));
	scanf("%s", p);
	if (strcmp(p, "#return#") == 0)
		return 0; //返回上一层

	ms->from = userid;
	ms->to = 0;
	ms->type = sql;
	sprintf(ms->value,
		"INSERT  INTO requests "
		"(requests.from,requests.to,requests.type,requests.how,requests."
		"value,requests.if_read)"
		"VALUES (%d,%d,%d,%d,\'%s\',%d);",
		userid, toid, msg, MESSAGES, p, 0);
	ms = cli_send_recv(ms, HUP_NO);
	if (ms == NULL)
		return 0;
	if (ms)
		free(ms);
	return 1; //继续聊天
}

void recv_secret_message(int toid)
{
	info *ms = (info *)malloc(sizeof(info));
	ms->from = userid;
	ms->to = 0;
	ms->type = sql;
	sprintf(ms->value,
		"select requests.add_time,requests.value from requests,relationship  "
		"where requests.to= %d and requests.how=%d and requests.from=%d and "
		"requests.from=relationship.id_2 and "
		"requests.to=relationship.id_1  and relationship.if_shield=0 and "
		"requests.if_read=0;",
		userid, MESSAGES, toid); //未屏蔽,未读的消息

	ms = cli_send_recv(ms, SHOW_MESSAGES);
	if (ms == NULL)
		return;
	if (atoi(ms->value) != 0) {
		char *buf = strchr(ms->value, '\n');
		buf++;
		printf("\nfrom %d :%s", toid, buf);
	}
	memset(ms->value, 0, BUFLEN);
	sprintf(ms->value,
		"update requests,relationship set requests.if_read=1 "
		"where requests.to= %d and requests.how=%d and requests.from=%d and "
		"requests.from=relationship.id_2 and "
		"requests.to=relationship.id_1  and relationship.if_shield=0 and "
		"requests.if_read=0;",
		userid, MESSAGES, toid); //设为已读
	ms->from = userid;
	ms->to = 0;
	ms->type = sql;
	ms = cli_send_recv(ms, HUP_NO);
	if (ms == NULL)
		return;
	if (ms)
		free(ms);
}

void show_secret_message(int toid)
{
	info *ms = (info *)malloc(sizeof(info));
	ms->from = userid;
	ms->to = 0;
	ms->type = sql;
	sprintf(ms->value,
		"select requests.add_time,requests.value from requests,relationship  "
		"where requests.to= %d and requests.how=%d and requests.from=%d and "
		"requests.from=relationship.id_2 and "
		"requests.to=relationship.id_1  ;",
		userid, MESSAGES, toid); //未屏蔽,未读的消息

	ms = cli_send_recv(ms, SHOW_MESSAGES);
	if (ms == NULL)
		return;
	int number = atoi(ms->value);
	printf("have %d messages:\n", number);
	if (number != 0) {
		char *buf = strchr(ms->value, '\n');
		buf++;
		printf("\nfrom %d :%s", toid, buf);
	}
	if (ms)
		free(ms);
}

void message_menu(int toid)
{
	int returnnumber = 1;
	printf("随时输入(#return#)返回上一层\n");
	while (returnnumber) {
		recv_secret_message(toid);
		returnnumber = send_secret_message(toid);
	}
	return;
}
