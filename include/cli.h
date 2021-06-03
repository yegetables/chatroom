#include "cJSON.h"
#include "mysql/mysql.h"
#include "sdebug.info.h"
#include "wrap.h"
#include "zlog.h"
#define BUFLEN 4096
#define MAXCLIENT 5000

/// @enum bool枚举
typedef enum
{
    true  = 1,
    false = 0
} bool;
typedef enum
{
    msg,
    file,
    sql
} value_type;

typedef struct
{
    char value[BUFLEN];
    value_type type;
    int from;
    int to;
} info;

void clienthelp(void);

void justwrite(int, int, void *args);

// void showevents(void *args, int line, char *fun);

zlog_category_t *my_zlog_init(char *);

bool cli_accessusername(char *name);
bool cli_accesspasswd(char *name, char *passwd);
bool useronline(char *name);
void showmainmenu(void);