#include "config.h"
#include PROJECT_CLIENTHEAD
extern int show_line;

void move_clln(int nums)
{
    for (int i = 0; i < nums; i++)
    {
        move_cll;
        printf("                             \n");
    }
    move_up(nums);
}

void re_show(void)
{
    if (show_line < 0)
        show_line = 0;
    if (show_line == 0)
    {
        information_bar();
        show_line += 3;
        return;
    }
    if (show_line > 0 && show_line < 3)
    {
        printf("showline %d error\n", show_line);
        exit(1);
    }
    // move_save;
    //大于3

    move_up(show_line);   //移动到开头
    move_clln(show_line); //清空
    information_bar();
    show_line = 3;
    // printf("%d   %d      %d\n", ++show_line);
}