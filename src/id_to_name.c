#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
extern zlog_category_t *ser;
extern MYSQL *sql_l;
#else
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
#endif

char *id_to_name(int id, char *name)
{
    if (name == NULL) return NULL;
#ifdef PROJECT_SERVER
    char p[BUFLEN] = {0};
    sprintf(p, "select user_name from user where user_id =\'%d\';", id);
    mysql_query(sql_l, p);
    MYSQL_RES *result = mysql_store_result(sql_l);
    int returnnumber  = mysql_num_rows(result);
    if (1 == returnnumber)
    {
        MYSQL_ROW rowline = mysql_fetch_row(result);
        strcpy(name, rowline[0]);
        mysql_free_result(result);
    }
    else
    {
        zlog_error(ser, "id not only %d", id);
        mysql_free_result(result);
        exit(-1);
    }
#else
#endif
    return name;
}

int name_to_id(char *name)
{
    int a = -1;
    if (name == NULL) return a;
#ifdef PROJECT_SERVER
    char p[BUFLEN] = {0};
    sprintf(p, "select user_id from user where user_name =\'%s\';", name);
    mysql_query(sql_l, p);
    MYSQL_RES *result = mysql_store_result(sql_l);
    int returnnumber  = mysql_num_rows(result);
    if (1 == returnnumber)
    {
        MYSQL_ROW rowline = mysql_fetch_row(result);
        a                 = atoi(rowline[0]);
        mysql_free_result(result);
    }
    else
    {
        zlog_error(ser, "name not only %s", name);
        mysql_free_result(result);
        exit(-1);
    }

#else
    return a;
#endif
}