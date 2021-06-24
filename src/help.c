#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
#else
#include PROJECT_CLIENTHEAD
#endif
void help(void)
{
#ifdef PROJECT_SERVER

	printf("***********使用说明***********\n");
	printf("-h ----------帮助手册\n");

#else
	printf("***********使用说明***********\n");
	printf("-h ----------帮助手册\n");
	printf("userage : . exec ipaddr port \n ./a.out 127.0.0.1 5000\n");

#endif
	return;
}