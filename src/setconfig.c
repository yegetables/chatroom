#include "../include/ser.h"
extern int port;
char logconf[30] = "./log/s.log.conf";
void setconfig(void)
{
    const char *configpath = "./config.json";  //写死配置路径
    int configfd;
    char bufjson[BUFLEN] = {0};
    int i                = 0;
    //读配置文件
    if (!access(configpath, F_OK) && !access(configpath, R_OK))
    {
        configfd = open(configpath, O_RDONLY);
        int n    = 1;
        while (n != 0)
        {
            n = read(configfd, bufjson, BUFLEN);
        }

        {
            cJSON *configjson = cJSON_Parse(bufjson);
            if (configjson == NULL)
            {
                perror("not json ");
                exit(-1);
            }

            cJSON *item = NULL;
            char *name  = NULL;
            name        = "port";
            if (cJSON_HasObjectItem(configjson, name) &&
                cJSON_IsNumber((item = cJSON_GetObjectItem(configjson, name))))
            {
                port = item->valueint;
            }

            name = "logconf";
            if (cJSON_HasObjectItem(configjson, name) &&
                cJSON_IsString((item = cJSON_GetObjectItem(configjson, name))))
            {
                memset(logconf, 0, sizeof(logconf));
                strcpy(logconf, item->valuestring);
            }
            cJSON_Delete(configjson);
        }

        close(configfd);
    }
    else
    {
        perror("config.json can't read ");
        exit(-1);
    }
}
