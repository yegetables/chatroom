#include "config.h"
#include PROJECT_SERVERHEAD

extern int port;
void setconfig(void)
{
    int configfd;
    char bufjson[BUFLEN] = {0};
  
    //读配置文件
    if (!access(PROJECT_CONFIG, F_OK) && !access(PROJECT_CONFIG, R_OK))
    {
        configfd = open(PROJECT_CONFIG, O_RDONLY);
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

            // name = PROJECT_LOGPROJECT_CONFIG;
            // if (cJSON_HasObjectItem(configjson, PROJECT_LOGPROJECT_CONFIG) &&
            //     cJSON_IsString((item = cJSON_GetObjectItem(configjson,
            //     name))))
            // {
            //     strcpy(PROJECT_LOGPROJECT_CONFIG, item->valuestring);
            // }
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
