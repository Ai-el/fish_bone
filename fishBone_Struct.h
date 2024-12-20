
#ifndef __FISHBONE_STRUCT_H_
#define __FISHBONE_STRUCT_H_

#include "stdio.h"
#include "stdlib.h"
#include <stdint.h>

#define CHILD_NODE_COUNT_MAX 10
#define PAGE_NAME_SIZE_MAX 20

#define fb_malloc malloc
#define fb_free free

typedef void (*fb_probe)(void *arg);    // 加载函数  组件生成
typedef void (*fb_drawMain)(void *arg); // 渲染函数  输出图像
typedef void (*fb_remove)(void *arg);   // 退出函数 释放内存
typedef void *(*fb_Singal)(void *arg);  // 事件返回信号 return下歌页面路径

typedef struct fb_operations
{
    void (*probe)(void *arg);        // 加载函数
    void (*draw_main)(void *arg);    // 渲染函数 循环
    void (*remove)(void *arg);       // 退出函数
    void *(*inputSingal)(void *arg); // 事件信号
} fb_operations;

int FishBone_pages_init(void);                  // 组件初始化 默认为root
int fb_rootPages_init(fb_operations *root_ope); // 组件初始化 自定义操作组件

void fb_root_mangeTask(void *arg); // 主线任务

int fb_nodeCreate(uint8_t *pageName, uint8_t *targetPath,
                  fb_probe probe_funktion, fb_drawMain funktion, fb_Singal signal, fb_remove remove); // 新节点创建 舍弃
int fb_pageCreate_ope(uint8_t *pageName, uint8_t *parentPath, fb_operations *ope);                    // 新页面创建，使用操作集合绑定操作函数

// 这里进行页面名字，路径 每创建  root 为根节点
//-------------page_name----------------------
#define _name_root "root"
#define _name_menu "CN_menu"
#define _name_ENmenu "EN_menu"
#define _name_test1 "test1"
#define _name_test2 "test2"
#define _name_test3 "test3"
#define _name_test4 "test4"
#define _name_test5 "test5"
#define _name_test6 "test6"
//--------------jamp_target_path---------------
#define _path_Rage_root "root"

#define _path_page_menu "root/CN_menu"
#define _path_page_test1 "root/CN_menu/test1"
#define _path_page_test2 "root/CN_menu/test2"
#define _path_page_test3 "root/CN_menu/test3"

#define _path_page_ENmenu "root/EN_menu"
#define _path_page_test4 "root/EN_menu/test1"
#define _path_page_test5 "root/EN_menu/test2"
#define _path_page_test6 "root/EN_menu/test3"

#define _path_page_test_child _path_page_menu
//-------------user's pages define-------------
#ifdef _FB_USER_PAGES_H_
#include "fb_user_pages.h"
#endif

/*
初始化后，先把页面添加完成后，再进入主函数，主函数是一个循环，可以用作线程或者主循环
*/
#endif