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
        how[0] = "[IF_HAS]";
        how[1] = "[IF_DONE]";
        how[2] = "[MANY_RESULT]";
        how[3] = "[WHAT_FIRST_VALUE]";
        how[4] = "[SET_ONLINE]";
        // strcpy(how[0], "[IF_HAS]");
        // strcpy(how[1], "[IF_DONE]");
        // strcpy(how[2], "[MANY_RESULT]");
        // strcpy(how[3], "[WHAT_FIRST_VALUE]");
        // strcpy(how[4], "[SET_ONLINE]");
    }
    static char *type[5];
    {
        type[0] = "[msg]";
        type[1] = "[file]";
        type[2] = "[sql]";
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
