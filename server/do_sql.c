#include "config.h"
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
extern MYSQL *sql_l;
extern char fd_name[MAXCLIENT][30];
extern int fd_id[MAXCLIENT];
bool do_sql(events *ev)
{
	info *ms = &ev->js;
	int aasd = 2;
	switch (ms->how) {
	//显式更改from to
	case SET_ONLINE: { //设置在线
		if (!base_sql_query(ms))
			return false;
		if (!event_set_online(ev))
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case GET_MESSAGE_FROM: {
		aasd = 1;
		if (!base_sql_query(ms))
			return false;
		if (base_GET_MANY_VALUE(ms, aasd) < 0)
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case SHOW_GROUP_MESSAGES: {
		aasd = 3;
		if (!base_sql_query(ms))
			return false;
		if (base_GET_MANY_VALUE(ms, aasd) < 0)
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case SHOW_APPLY:
	case FR_LIST:
	case SHOW_MESSAGES:
	case GET_MANY_VALUE: {
		aasd = 2;
		if (!base_sql_query(ms))
			return false;
		if (base_GET_MANY_VALUE(ms, aasd) < 0)
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case GR_LIST: {
		if (get_authority(ms->from, ms->to) > NO_PEOPLE) {
			if (!base_sql_query(ms))
				return false;
			if (base_GET_MANY_VALUE(ms, aasd) < 0)
				return false;
		} else {
			strcpy(ms->value, "-1");
		}
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case SHOW_GROUP_APPLY: {
		event_show_group_apply(ms);
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case AGREE_RECV_FILE: {
		if (!base_sql_query(ms))
			return false;
		if (base_GET_MANY_VALUE(ms, 2) < 0)
			return false;
		if (!event_AGREE_RECV_FILE(ms))
			return false;
		break;
	}
	case AGREE_APPLICATION: {
		if (!base_sql_query(ms))
			return false;
		if (!case_WHAT_FIRST_VALUE(ms))
			return false;
		if (!base_sql_query(ms))
			return false;
		if (!case_IF_DONE(ms))
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case CREATE_GROUP: {
		if (!base_sql_query(ms))
			return false;
		if (!event_CREATE_GROUP(ms))
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	// 优化其他多次查询,一次返回自增主键
	case DEL_GROUP: {
		// if (!base_sql_query(ms))
		// 	return false;
		if (!event_DEL_GROUP(ms)) {
			strcpy(ms->value, "-1");
		} else {
			strcpy(ms->value, "1");
		}
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case SEND_FILE_REDY: {
		if (event_recv_file_ready(ev) < 0)
			return false;
		//插入数据到数据库
		if (!base_sql_query(&ev->js))
			return false;
		if (!case_IF_DONE(ms))
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case IF_DONE: {
		if (!base_sql_query(ms))
			return false;
		if (!case_IF_DONE(ms))
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case ADD_GROUP: {
		event_ADD_GROUP(ms);
		strcpy(ms->value, "1");
		ms->to = ms->from;
		ms->from = 0;
		break;
	}

	case ADD_GROUP_APPLY: {
		event_ADD_GROUP_APPLY(ms);
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case set_POWER_GROUP: {
		event_set_POWER_GROUP(ms);
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case EXIT_GROUP: {
		event_exit_GROUP(ms);
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case IF_HAS: {
		if (!base_sql_query(ms))
			return false;
		if (!case_IF_HAS(ms))
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case MANY_RESULT: {
		if (!base_sql_query(ms))
			return false;
		if (!case_MANY_RESULT(ms))
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case WHAT_FIRST_VALUE: {
		if (!base_sql_query(ms))
			return false;
		if (!case_WHAT_FIRST_VALUE(ms))
			return false;
		ms->to = ms->from;
		ms->from = 0;
		break;
	}
	case HUP_NO: {
		if (!base_sql_query(ms))
			return false;
		ms->to = ms->from = 0;
		break;
	}
	default: {
		zlog_warn(ser, "don't know which how");
		return false;
	}
	}
	return true;

	{ // mysql_store_result保存查询到的数据到result
	  // mysql_num_rows返回结果集中的行数
	  // MYSQL_ROW *row = mysql_fetch_row();
	  // while (row = mysql_fetch_row(res))
	  // {
	  //     for (t = 0; t < mysql_num_fields(res); t++)

		//     {
		//         printf(
		//             "%s"
		//             " ",
		//             row[t]);
		//     }

		//     printf("%s", row[1]);
		//     //表示查询的第几个字段的！也还可以遍历全部字段信息
		// }
	}
}

bool base_sql_query(info *ms)
{
	char *buf = ms->value;
	int returnnumber;
	returnnumber = -1;
	returnnumber = mysql_query(sql_l, buf);
	if (returnnumber) //出错
	{
		zlog_error(ser, "执行%s时出现异常: %s", ms->value,
			   mysql_error(sql_l));
		return false;
	} else
		return true;
}

bool case_WHAT_FIRST_VALUE(info *ms)
{
	char *buf = ms->value;
	MYSQL_RES *result = NULL;
	MYSQL_ROW rowline;
	result = mysql_store_result(sql_l);
	if (result == NULL) //出错
	{
		zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
		mysql_free_result(result);
		return false;
	}
	memset(buf, 0, BUFLEN);
	rowline = mysql_fetch_row(result); //第一行
	strcpy(buf, rowline[0]); //第一列
	mysql_free_result(result);
	return true;
}

bool case_MANY_RESULT(info *ms)
{
	char *buf = ms->value;
	MYSQL_RES *result = NULL;
	result = mysql_store_result(sql_l);
	if (result == NULL) //出错
	{
		zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
		mysql_free_result(result);
		return false;
	}
	memset(buf, 0, BUFLEN);
	itoa(mysql_num_rows(result), buf, 10);
	mysql_free_result(result);
	return true;
}

bool case_IF_HAS(info *ms)
{
	char *buf = ms->value;
	MYSQL_RES *result = NULL;
	result = mysql_store_result(sql_l);
	if (result == NULL) //出错
	{
		zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
		mysql_free_result(result);
		return false;
	}
	memset(buf, 0, BUFLEN);
	if (mysql_num_rows(result))
		strcpy(buf, "1");
	else
		strcpy(buf, "0");
	mysql_free_result(result);
	return true;
}

bool case_IF_DONE(info *ms)
{
	char *buf = ms->value;
	memset(buf, 0, BUFLEN);
	strcpy(buf, "1");
	return true;
}

int base_GET_MANY_VALUE(info *ms, int fetch)
{
	char *buf = ms->value;
	int number = 0;
	memset(buf, 0, BUFLEN);

	MYSQL_RES *result = NULL;
	MYSQL_ROW rowline;
	result = mysql_store_result(sql_l);
	if (result == NULL) //出错
	{
		zlog_error(ser, "result时出现异常: %s", mysql_error(sql_l));
		mysql_free_result(result);
		return -1;
	}

	number = mysql_num_rows(result);
	sprintf(buf, "%d\n", number);
	int i = 0;
	int j = 0;
	for (; i < number; i++) {
		rowline = mysql_fetch_row(result); //第i+1行
		for (j = 0; j < fetch; j++) {
			if (j != fetch - 1)
				sprintf(&buf[strlen(buf)], "%s ", rowline[j]);
			else
				sprintf(&buf[strlen(buf)], "%s\n", rowline[j]);
		}
	}
	// zlog_error(ser, "get value buf:%s fetch:%d number:%d", buf,
	// fetch, number);
	mysql_free_result(result);
	return number;
}

bool event_set_online(events *ev)
{ // get name(设置status=1)
	info *ms = &ev->js;
	char *buf = ms->value;
	{ //获取name更改为查询id
		char na[30] = { 0 };
		sscanf(buf,
		       "update user set  user_status= \'1\'  where user_name= \'%s\' ;",
		       na);
		na[strlen(na) - 1] = 0;
		// zlog_debug(ser, "na = %s", na);
		memset(buf, 0, BUFLEN);
		sprintf(buf,
			"select user_id  from user where user_name=\'%s\';",
			na);
	}
	if (!base_sql_query(ms))
		return false;
	//将id返回
	if (!case_WHAT_FIRST_VALUE(ms))
		return false;
	{ //设置在线状态
		int id = atoi(buf);
		fd_id[ev->fd] = id;
		id_to_name(id, fd_name[ev->fd]); // fd->name
		zlog_info(ser, "**online**   fd:%d id:%d name:%s", ev->fd,
			  fd_id[ev->fd], fd_name[ev->fd]);
	}

	return true;
}
