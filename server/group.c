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
	}
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

	sprintf(ms->value, "%d", groupid);
	return true;
}
