#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
#else
#include PROJECT_CLIENTHEAD
#endif

zlog_category_t *my_zlog_init(char *category)
{
	if (zlog_init(PROJECT_LOGCONFIG)) {
		perror("zlog_init error");
		exit(-1);
	}

	zlog_category_t *ser = zlog_get_category(category);
	if (!ser) {
		zlog_fini();
		perror("zlog_get_category error");
		exit(-1);
	}
	return ser;
}
