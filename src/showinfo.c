#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
#else
#include PROJECT_CLIENTHEAD
#endif

char *showinfo(info *ms)
{
    static char *how[20];
    {
        how[0]  = "[HUP_NO]";
        how[1]  = "[IF_HAS]";
        how[2]  = "[IF_DONE]";
        how[3]  = "[MANY_RESULT]";
        how[4]  = "[WHAT_FIRST_VALUE]";
        how[5]  = "[SET_ONLINE]";
        how[6]  = "[FR_LIST]";
        how[7]  = "[ADD_FRIEND]";
        how[8]  = "[MESSAGES]";
        how[9]  = "[SHOW_APPLY]";
        how[10] = "[GET_MANY_VALUE]";
        how[11] = "[AGREE_APPLICATION]";
        how[12] = "[DEL_SELECT]";
        how[13] = "[SHOW_MESSAGES]";
        how[14] = "[14]";
        how[15] = "[15]";
        how[16] = "[16]";
        how[17] = "[17]";
        how[18] = "[???]";
        how[19] = "[???]";

        // strcpy(how[0], "[IF_HAS]");
        // strcpy(how[1], "[IF_DONE]");
        // strcpy(how[2], "[MANY_RESULT]");
        // strcpy(how[3], "[WHAT_FIRST_VALUE]");
        // strcpy(how[4], "[SET_ONLINE]");
    }
    static char *type[10];
    {
        type[1] = "[msg]";
        type[2] = "[file]";
        type[3] = "[sql]";
        type[4] = "[request]";
        type[5] = "[5]";
        type[6] = "[6]";

        // strcpy(type[0], "[msg]");
        // strcpy(type[1], "[file]");
        // strcpy(type[2], "[sql]");
    }
    char *logbuf = (char *)calloc(BUFLEN, sizeof(char));

    sprintf(&logbuf[strlen(logbuf)],
            "----------info---------\n"
            "{\nfrom :%d to :%d   ",
            ms->from, ms->to);

    sprintf(&logbuf[strlen(logbuf)], "%s", type[ms->type]);
    sprintf(&logbuf[strlen(logbuf)], "%s", how[ms->how]);
    sprintf(&logbuf[strlen(logbuf)], "\n%s\n}", ms->value);
    return logbuf;
}
