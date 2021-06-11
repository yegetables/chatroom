#include "config.h"
#include PROJECT_CLIENTHEAD

void entermenu(void)
{
    showmainmenu();
    int c = getchar();
    return;
}
void showmainmenu(void)
{
    printf("\n-------------------\n");
    printf("1.好友列表\n");  //在线?昵称?
    printf("2.群聊列表\n");

    printf("3.进入聊天界面\n");

    printf("4.个人设置\n");
    // printf("8.找回密码\n");//登陆界面
    printf("5.好友设置\n");
    printf("\n-------------------\n");
    // printf("2.添加好友\n");
    // printf("3.删除好友\n");
    // 好友的添加、删除、查询
    // 查看好友列表
    // 显示好友状态（在线与否）
}
