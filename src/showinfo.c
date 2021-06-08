#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
#else
#include PROJECT_CLIENTHEAD
#endif

char *showinfo(info *ms)
{
    char *logbuf = (char *)calloc(BUFLEN, sizeof(char));

    sprintf(&logbuf[strlen(logbuf)],
            "\n----------info---------\n"
            "{\nfrom :%d to :%d   \n",
            ms->from, ms->to);

    if (ms->type == msg) sprintf(&logbuf[strlen(logbuf)], "[msg]");
    if (ms->type == file) sprintf(&logbuf[strlen(logbuf)], "[file]");
    if (ms->type == sql) sprintf(&logbuf[strlen(logbuf)], "[sql]");
    sprintf(&logbuf[strlen(logbuf)], "%s\n}", ms->value);
    return logbuf;
}
