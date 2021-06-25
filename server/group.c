#include "config.h"
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
extern MYSQL *sql_l;
extern char fd_name[MAXCLIENT][30];
extern int fd_id[MAXCLIENT];

bool event_DEL_GROUP(info *ms)
{
	int group_id = -1;
	int user_id = -1;
	sscanf(ms->value,
	       "select * from groups where  group_id=%d  and master_id=%d;",
	       &group_id, &user_id);
	// zlog_info(ser, "get groupid=%d,masterid=%d", group_id,
	// user_id);
	if (!case_MANY_RESULT(ms))
		return false;
	int result = atoi(ms->value);
	// zlog_debug(ser, " result =%d", result);
	if (atoi(ms->value) == 1) {
		sprintf(ms->value,
			"delete from groups where  group_id=%d  and master_id=%d;",
			group_id, user_id);
		if (!base_sql_query(ms))
			return false;
		strcpy(ms->value, "1");
	} else {
		// memset(ms->value, 0, BUFLEN);
		sprintf(ms->value, "-1");
		return true;
	}

	//清除所有关系
	del_relationship(&group_id, NULL);
	del_relationship(NULL, &group_id);

	return true;
}

bool event_CREATE_GROUP(info *ms)
{
	// memset(ms->value, 0, BUFLEN);
	sprintf(ms->value, "SELECT LAST_INSERT_ID();");
	if (!base_sql_query(ms))
		return false;
	if (!case_WHAT_FIRST_VALUE(ms))
		return false;
	int groupid = atoi(ms->value);

	// add relationship
	set_relationship(ms->from, groupid, OWNER | NO_SHIELD);
	set_relationship(groupid, ms->from, TALK);

	sprintf(ms->value, "%d", groupid);
	return true;
}

void set_relationship(int id1, int id2, int is)
{
	char p[BUFLEN] = { 0 };
	sprintf(p, "select * from relationship where id_1=%d and id_2=%d;", id1,
		id2);
	int returnnumber;
	returnnumber = -1;
	returnnumber = mysql_query(sql_l, p);
	if (returnnumber) //出错
	{
		zlog_error(ser, "执行时出现异常: %s", mysql_error(sql_l));
		return;
	}
	MYSQL_RES *result = NULL;
	result = mysql_store_result(sql_l);
	if (result == NULL) //出错
	{
		zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
		mysql_free_result(result);
		return;
	}
	memset(p, 0, BUFLEN);
	if (mysql_num_rows(result) == 0) {
		sprintf(p,
			"insert into relationship (id_1,id_2,if_shield)"
			"values(%d,%d,%d);",
			id1, id2, is);

	} else if (mysql_num_rows(result) > 1) {
		zlog_error(ser, "resule more");
		return;
	} else //修改
	{
		sprintf(p,
			"update relationship set if_shield=%d where"
			"id_1=%d and id_2=%d;",
			is, id1, id2);
	}

	returnnumber = -1;
	returnnumber = mysql_query(sql_l, p);
	if (returnnumber) //出错
	{
		zlog_error(ser, "执行时出现异常: %s", mysql_error(sql_l));
		return;
	}
}

void del_relationship(int *id1, int *id2)
{
	char p[BUFLEN] = { 0 };

	if (id1 != NULL && id2 != NULL) {
		sprintf(p,
			"delete  from relationship where id_1=%d and id_2=%d ;",
			*id1, *id2);
	} else if (id1 == NULL && id2 != NULL) {
		sprintf(p, "delete  from relationship where  id_2=%d ;", *id2);
	} else if (id1 != NULL && id2 == NULL) {
		sprintf(p, "delete  from relationship where id_1=%d  ;", *id1);
	} else {
		return;
	}
	// base_sql_query(ms);
	int returnnumber;
	returnnumber = -1;
	returnnumber = mysql_query(sql_l, p);
	if (returnnumber) //出错
	{
		zlog_error(ser, "执行时出现异常: %s", mysql_error(sql_l));
		return;
	}
}
