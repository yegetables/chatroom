#include "config.h"
#include PROJECT_CLIENTHEAD
extern zlog_category_t *cli;
extern int cfd;
extern int userid;
bool cli_register(char *name, char *passwd, char *email)  // IF_DONE
{
    if (name == NULL || passwd == NULL || email == NULL) return false;
    char p[BUFLEN] = {0};
    sprintf(p,
            "INSERT INTO user (user_name,user_passwd, user_email,user_status) "
            "VALUES ( "
            "\'%s\',\'%s\',\'%s\',0);",
            name, passwd, email);
    info *ms = NULL;
    ms = cli_creatinfo(userid, 0, sql, REGISTER, p);
    if (ms == NULL) return false;
    int s = atoi(ms->value);
    if (ms) free(ms);
    if (s)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool cli_accesspasswd(char *name, char *passwd)  // IF_HAS
{
    if (name == NULL) return false;

    char p[BUFLEN] = {0};

    if (passwd == NULL)
    {
        sprintf(p, "select * from user where user_name =\'%s\';", name);
    }
    else
    {
        for (int i = 0; i < strlen(passwd); i++)
        {
            passwd[i] = passwd[i] + 6;
        }
        sprintf(p,
                "select * from user where user_name =\'%s\' and "
                "user_passwd=\'%s\';",
                name, passwd);
    }
    info *ms = cli_creatinfo(userid, 0, sql, IF_HAS, p);
    if (ms == NULL) return false;
    int s = atoi(ms->value);
    if (ms) free(ms);
    if (s)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool cli_accessonline(char *name)  // WHAT_FIRST_VALUE
{
    if (name == NULL) return false;

    char p[BUFLEN] = {0};

    sprintf(p, "SELECT user_status  FROM user where user_name =\'%s\';", name);
    info *ms = cli_creatinfo(userid, 0, sql, WHAT_FIRST_VALUE, p);
    if (ms == NULL) return false;
    int s = atoi(ms->value);
    if (ms) free(ms);
    if (s)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int cli_setonline(char *name)  // SET_ONLINE
{
    if (name == NULL) return -1;
    char p[BUFLEN] = {0};
    sprintf(p, "update user set  user_status= \'1\'  where user_name= \'%s\' ;", name);
    info *ms = cli_creatinfo(userid, 0, sql, SET_ONLINE, p);
    if (ms == NULL) return -1;
    int s = atoi(ms->value);
    if (ms) free(ms);
    if (s > 0)
    {
        return s;
    }
    else
    {
        return -1;
    }
}

bool IsValidEmail(char *s)
{
    char *ms;
    if ((ms = strchr(s, '@')) == NULL)
    {
        return false;
    }
    if (strchr(ms + 1, '@') != NULL)
    {
        return false;
    }
    if (strchr(ms + 1, '.') == NULL)
    {
        return false;
    }
    if (strchr(s, '.') < ms)
    {
        if (strchr(strchr(s, '.') + 1, '.') < ms)
        {
            return false;
        }
    }
    if (strlen(strrchr(s, '.') + 1) > 4 || strlen(strrchr(s, '.') + 1) < 2)
    {
        return false;
    }
    return true;
}
