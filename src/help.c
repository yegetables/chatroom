#include "config.h"
#ifdef PROJECT_SERVER
#include PROJECT_SERVERHEAD
#else
#include PROJECT_CLIENTHEAD
extern int show_line;
#endif
void help(void)
{
#ifdef PROJECT_SERVER

    printf("***********使用说明***********\n");
    printf("-h ----------帮助手册\n");

#else
    show_line += 3;
    printf("***********使用说明***********\n");
    printf("-h ----------帮助手册\n");
    printf("userage : . exec ipaddr port \n ./a.out 127.0.0.1 5000\n");

#endif
    return;
}

int getch()
{
    int c = 0;
    struct termios org_opts, new_opts;
    int res = 0;
    //-----  store old settings -----------
    res = tcgetattr(STDIN_FILENO, &org_opts);
    assert(res == 0);
    //---- set new terminal parms --------
    memcpy(&new_opts, &org_opts, sizeof(new_opts));
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    c = getchar();
    //------  restore old settings ---------
    res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res == 0);
    // printf("*");
    return c;
}