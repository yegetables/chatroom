#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
extern MYSQL *sql_l;
#else
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int userid;
#endif

char *id_to_name(int id, char *name)
{
	if (name == NULL)
		return NULL;
#ifdef PROJECT_SERVER
	char p[BUFLEN] = { 0 };
	sprintf(p, "select user_name from user where user_id =\'%d\';", id);
	mysql_query(sql_l, p);
	MYSQL_RES *result = mysql_store_result(sql_l);
	int returnnumber = mysql_num_rows(result);
	if (1 == returnnumber) {
		MYSQL_ROW rowline = mysql_fetch_row(result);
		strcpy(name, rowline[0]);
		mysql_free_result(result);
	} else {
		zlog_error(ser, "id not only %d", id);
		mysql_free_result(result);
		exit(-1);
	}

#else

	info *ms = (info *)malloc(sizeof(info));
	{
		sprintf(ms->value,
			"select user_name from user where user_id=\'%d\';", id);
		ms->type = sql;
		ms->from = userid;
		ms->to = 0;
	}
	ms = cli_send_recv(ms, WHAT_FIRST_VALUE);
	if (ms == NULL) {
		zlog_error(cli, "recv error");
		return NULL;
	}
	strcpy(name, ms->value);
	if (ms)
		free(ms);

#endif
	return name;
}

int name_to_id(char *name)
{
	int a = -1;
	if (name == NULL)
		return a;
#ifdef PROJECT_SERVER
	char p[BUFLEN] = { 0 };
	sprintf(p, "select user_id from user where user_name =\'%s\';", name);
	mysql_query(sql_l, p);
	MYSQL_RES *result = mysql_store_result(sql_l);
	int returnnumber = mysql_num_rows(result);
	if (1 == returnnumber) {
		MYSQL_ROW rowline = mysql_fetch_row(result);
		a = atoi(rowline[0]);
		mysql_free_result(result);
	} else {
		zlog_error(ser, "name not only %s", name);
		mysql_free_result(result);
		exit(-1);
	}

#else

	info *ms = (info *)malloc(sizeof(info));
	{
		sprintf(ms->value,
			"select user_id from user where user_name=\'%s\';",
			name);
		ms->type = sql;
		ms->from = userid;
		ms->to = 0;
	}
	ms = cli_send_recv(ms, WHAT_FIRST_VALUE);
	if (ms == NULL) {
		zlog_error(cli, "recv error");
		return -1;
	}
	a = atoi(ms->value);
	if (ms)
		free(ms);

#endif
	return a;
}

char *itoa(int num, char *str, int radix)
{ /*索引表*/
	char index[] = "0123456789ABCDEF";
	unsigned unum; /*中间变量*/
	int i = 0, j, k;
	/*确定unum的值*/
	if (radix == 10 && num < 0) /*十进制负数*/
	{
		unum = (unsigned)-num;
		str[i++] = '-';
	} else
		unum = (unsigned)num; /*其他情况*/
	/*转换*/
	do {
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;
	} while (unum);
	str[i] = '\0';
	/*逆序*/
	if (str[0] == '-')
		k = 1; /*十进制负数*/
	else
		k = 0;

	for (j = k; j <= (i - 1) / 2; j++) {
		char temp;
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - 1 + k - j] = temp;
	}
	return str;
}