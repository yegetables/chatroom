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
	// // zlog_info(ser, "get groupid=%d,masterid=%d", group_id,
	// // user_id);
	// if (!case_MANY_RESULT(ms))
	// 	return false;
	// int result = atoi(ms->value);
	// // zlog_debug(ser, " result =%d", result);
	// if (atoi(ms->value) == 1) {
	// 	sprintf(ms->value,
	// 		"delete from groups where  group_id=%d  and
	// master_id=%d;", 		group_id, user_id); 	if
	// (!base_sql_query(ms)) 		return
	// false; 	strcpy(ms->value, "1"); } else {
	// 	// memset(ms->value, 0, BUFLEN);
	// 	sprintf(ms->value, "-1");
	// 	return true;
	// }
	if (get_authority(user_id, group_id) == OWNER) //是群主
	{
		sprintf(ms->value,
			"delete from groups where  group_id=%d  and master_id=%d;",
			group_id, user_id);
		if (!base_sql_query(ms))
			return false;
		//清除所有关系
		del_relationship(&group_id, NULL);
		del_relationship(NULL, &group_id);
		return true;
	} else {
		return false;
	}
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

int set_relationship(int id1, int id2, int is)
{
	int ret = 0;
	char p[BUFLEN] = { 0 };
	sprintf(p,
		"select relationship.if_shield from relationship "
		" where id_1=%d and id_2=%d;",
		id1, id2);
	int returnnumber;
	returnnumber = -1;
	returnnumber = mysql_query(sql_l, p);
	if (returnnumber) //出错
	{
		zlog_error(ser, "执行%s时出现异常: %s", p, mysql_error(sql_l));
		return -1;
	}
	MYSQL_RES *result = NULL;
	result = mysql_store_result(sql_l);
	if (result == NULL) //出错
	{
		zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
		mysql_free_result(result);
		return -1;
	}
	memset(p, 0, BUFLEN);
	if (mysql_num_rows(result) == 0) {
		sprintf(p,
			"insert into relationship (id_1,id_2,if_shield)"
			"values(%d,%d,%d);",
			id1, id2, is);
		ret = 0;
	} else if (mysql_num_rows(result) > 1) {
		zlog_error(ser, "resule more");
		return -1;
	} else //修改
	{
		MYSQL_ROW rowline;
		rowline = mysql_fetch_row(result); //第一行
		ret = atoi(rowline[0]);
		sprintf(p,
			"update relationship set if_shield=%d where "
			"id_1=%d and id_2=%d;",
			is, id1, id2);
	}

	returnnumber = -1;
	returnnumber = mysql_query(sql_l, p);
	if (returnnumber) //出错
	{
		zlog_error(ser, "执行%s时出现异常: %s", p, mysql_error(sql_l));
		return -1;
	}
	return ret;
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

int get_authority(int userid, int groupid)
{
	char p[BUFLEN] = { 0 };
	sprintf(p,
		"select relationship.if_shield from relationship where "
		" relationship.id_1=%d and relationship.id_2=%d;",
		userid, groupid);
	int returnnumber;
	returnnumber = -1;
	returnnumber = mysql_query(sql_l, p);
	if (returnnumber) //出错
	{
		zlog_error(ser, "执行%s时出现异常: %s", p, mysql_error(sql_l));
		return -1;
	}

	MYSQL_RES *result = NULL;
	MYSQL_ROW rowline;
	result = mysql_store_result(sql_l);
	if (result == NULL) //出错
	{
		zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
		mysql_free_result(result);
		return -2;
	}
	int ret;
	if (mysql_num_rows(result) == 1) {
		rowline = mysql_fetch_row(result); //第一行
		ret = atoi(rowline[0]);
	} else {
		ret = 0; //未加入
	}
	mysql_free_result(result);
	if (ret >= 8)
		ret -= 8;
	//过滤屏蔽
	return ret;
}

bool event_set_POWER_GROUP(info *ms)
{
	int Authority = -1;
	int toid;
	sscanf(ms->value, "%d %d", &Authority, &toid);
	int o1 = get_authority(ms->from, ms->to);
	int o2 = get_authority(toid, ms->to);
	if (o1 == OWNER && o2 == PEOPLE && Authority == MANER) { //提权非2
		set_relationship(toid, ms->to, o2 | 2);
		strcpy(ms->value, "1");
		return true;

	} else if (o1 == OWNER && o2 == MANER &&
		   Authority == PEOPLE) { //降权与9
		set_relationship(toid, ms->to, o2 & 9);
		strcpy(ms->value, "1");
		return true;
	} else {
		strcpy(ms->value, "-1");
		return false;
	}
}

bool event_exit_GROUP(info *ms)
{
	int toid;
	sscanf(ms->value, "%d", &toid);
	int o1 = get_authority(ms->from, ms->to);
	int o2 = get_authority(toid, ms->to);

	if (o1 == NO_PEOPLE || o2 == NO_PEOPLE) {
		strcpy(ms->value, "-1");
	} else if (o2 == OWNER) {
		strcpy(ms->value, "-1");
	} else if (o1 == PEOPLE && o2 == MANER) {
		strcpy(ms->value, "-1");
	} else {
		del_relationship(&ms->to, &toid);
		del_relationship(&toid, &ms->to);
		strcpy(ms->value, "1");
		return true;
	}
	return false;
}

bool event_ADD_GROUP_APPLY(info *ms)
{
	int Authority = -1;
	int toid;
	sscanf(ms->value, "%d %d", &Authority, &toid);
	int o1 = get_authority(ms->from, ms->to);
	int o2 = get_authority(toid, ms->to);
	if (o2 == NO_PEOPLE && Authority == PEOPLE) { //加人
		sprintf(ms->value,
			"insert into requests"
			"(requests.from, requests.to,requests.type,"
			"requests.how,requests.value,requests.if_read)"
			"values(%d,%d,%d,%d,\'%s\',0);",
			toid, ms->to, sql, ADD_GROUP_APPLY, "");
		if (!base_sql_query(ms))
			return false;
		stpcpy(ms->value, "1");
		return true;
	} else {
		strcpy(ms->value, "-1");
		return false;
	}
}

bool event_ADD_GROUP(info *ms)
{
	int toid;
	int groupid;
	sscanf(ms->value, "%d %d", &toid, &groupid);
	set_relationship(groupid, toid, TALK);
	set_relationship(toid, groupid, NO_SHIELD | PEOPLE);
	return true;
}

bool event_show_group_apply(info *ms)
{
	int o1 = get_authority(ms->from, ms->to);
	if (o1 == MANER || o1 == OWNER) {
		if (!base_sql_query(ms))
			return false;
		if (base_GET_MANY_VALUE(ms, 2) < 0)
			return false;
		return true;
	} else {
		strcpy(ms->value, "-1");
		return false;
	}
}