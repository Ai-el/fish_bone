
#include <windows.h>
#include "fishBone_Struct.h"
#include "pages/page_menu.h"
#include "pages/page_test1.h"
#include "pages/page_test2.h" 


int main(int argc, char const *argv[])
{
    FishBone_pages_init();
    printf("create root node\n");
  
    page_menu_init();
    // sleep(1);
    page_test1_init();
    // sleep(1);
    page_test2_init();
    // sleep(1);
 
   
    LPDWORD threadIds;
    // 创建第一个线程
    HANDLE  threadHandles = CreateThread(
        NULL,              // 默认安全属性
        0,                 // 默认堆栈大小
        fb_root_mangeTask,        // 线程函数
        NULL,  // 传递给线程函数的参数
        0,                 // 默认创建标志
        &threadIds       // 线程ID
    );
    
    if (threadHandles== NULL) {
        printf("创建线程失败\n");
        return 1;
    }

    // 等待线程结束
    // WaitForMultipleObjects(1, threadHandles, TRUE, INFINITE);
    WaitForSingleObject(threadHandles, INFINITE);
    printf("thread is over\n");
    // 关闭线程句柄
    CloseHandle(threadHandles);

    return 0;
}
