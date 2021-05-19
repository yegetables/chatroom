#include "../include/ser.h"

extern char logconf[30];

zlog_category_t *my_zlog_init(char *cate)
{
    if (zlog_init(logconf))
    {
        perror("zlog_init error");
        exit(-1);
    }

    zlog_category_t *ser = zlog_get_category(cate);
    if (!ser)
    {
        zlog_fini();
        perror("zlog_get_category error");
        exit(-1);
    }
    return ser;
}
