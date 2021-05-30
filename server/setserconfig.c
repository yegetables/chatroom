#include "config.h"
#include PROJECT_SERVERHEAD

extern int port;
char database_name[20];
char database_user[20];
char database_passwd[100];
char database_ip[20];
int database_port = 3306;
void setserconfig(char *filename)
{
    int configfd;
    char bufjson[BUFLEN] = {0};

    //读配置文件
    if (!access(filename, F_OK) && !access(filename, R_OK))
    {
        configfd = open(filename, O_RDONLY);
        int n    = 1;
        while (n != 0)
        {
            n = read(configfd, bufjson, BUFLEN);
        }
        n = 0;
        {
            cJSON *configjson = cJSON_Parse(bufjson);
            if (configjson == NULL)
            {
                perror("not json ");
                exit(-1);
            }

            cJSON *item = NULL;
            char *name  = NULL;
            name        = "server_port";
            if (cJSON_HasObjectItem(configjson, name) &&
                cJSON_IsNumber((item = cJSON_GetObjectItem(configjson, name))))
            {
                port = item->valueint;
            }
            name = "database_port";
            if (cJSON_HasObjectItem(configjson, name) &&
                cJSON_IsNumber((item = cJSON_GetObjectItem(configjson, name))))
            {
                database_port = item->valueint;
            }

            // string
            {
                name = "database_passwd";
                if (cJSON_HasObjectItem(configjson, name) &&
                    cJSON_IsString(
                        (item = cJSON_GetObjectItem(configjson, name))))
                {
                    strcpy(database_passwd, item->valuestring);
                }

                name = "database_name";
                if (cJSON_HasObjectItem(configjson, name) &&
                    cJSON_IsString(
                        (item = cJSON_GetObjectItem(configjson, name))))
                {
                    strcpy(database_name, item->valuestring);
                }
                name = "database_user";
                if (cJSON_HasObjectItem(configjson, name) &&
                    cJSON_IsString(
                        (item = cJSON_GetObjectItem(configjson, name))))
                {
                    strcpy(database_user, item->valuestring);
                }
                name = "database_ip";
                if (cJSON_HasObjectItem(configjson, name) &&
                    cJSON_IsString(
                        (item = cJSON_GetObjectItem(configjson, name))))
                {
                    strcpy(database_ip, item->valuestring);
                }
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
