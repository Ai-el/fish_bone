
#include "../fishBone_Struct.h"
#include "windows.h"

HANDLE hSemaphore;         // 信号量句柄
static int sharedData = 0; // 共享数据
static void draw_thread(void *arg)
{
    char str[10]={0};
    printf("menu drew is ok\n");
    while (1)
    {
        scanf("%s", str);
        if (strstr(str, "test1") != NULL)
        {
            ReleaseSemaphore(hSemaphore, 1, NULL); // 释放信号量
            sharedData = 1;
            break;
        }
        if (strstr(str, "test2") != NULL)
        {
            ReleaseSemaphore(hSemaphore, 1, NULL); // 释放信号量
            sharedData = 2;
            break;
        }
        if (strstr(str, "quit") != NULL)
        {
            ReleaseSemaphore(hSemaphore, 1, NULL); // 释放信号量
            sharedData = 3;
            break;
        }
        printf("menu thread is running\n");
    }
}

void pagemenu_draw(void *aeg)
{
    // 创建线程
    // 一个执行的，一个信号的
    LPDWORD threadIds;
    // 创建第一个线程
    HANDLE threadHandles = CreateThread(
        NULL,        // 默认安全属性
        0,           // 默认堆栈大小
        draw_thread, // 线程函数
        NULL,        // 传递给线程函数的参数
        0,           // 默认创建标志
        &threadIds   // 线程ID
    );

    if (threadHandles == NULL)
    {
        printf("创建线程失败\n");
    }
    // 信号量
    //  创建信号量
    hSemaphore = CreateSemaphore(
        NULL, // 默认安全属性
        0,    // 初始计数
        1,    // 最大计数
        NULL  // 不用于跨进程同步
    );

    if (hSemaphore == NULL)
    {
        printf("CreateSemaphore failed (%d)\n", GetLastError());
        return 1;
    }

    // WaitForSingleObject(hThread1, INFINITE);
}
static void *pagemenu_singal(void *arg)
{

    // 这里等待信号量
    WaitForSingleObject(hSemaphore, INFINITE);

    if (sharedData == 1)
    {
        sharedData = 0;
        goto page1;
    }
    else if (sharedData == 2)
    {
        sharedData = 0;
        goto page2;
    }
    else if (sharedData == 3)
    {
        sharedData = 0;
        goto page3;
    }

page3:
    CloseHandle(hSemaphore);
    return _path_Rage_root;

page2:
    CloseHandle(hSemaphore);
    return _path_page_test2;

page1:
    CloseHandle(hSemaphore);
    return _path_page_test1;
}

void page_menu_init(void)
{

    if (fb_nodeCreate(_name_menu, _path_Rage_root, NULL,pagemenu_draw, pagemenu_singal,NULL)) // 节点创建
    {
        printf(" page[%s] create fail\n",_name_menu);
    }else{
        printf(" page[%s] create success\n",_name_menu);
    }
}
