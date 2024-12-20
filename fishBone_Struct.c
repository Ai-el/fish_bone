
#include "fishBone_Struct.h"
#include "string.h"
#include "fb_handware.h"

typedef struct _s_FB_PageNode _s_FB_PageNode;
typedef struct _s_FB_PageNode
{
    _s_FB_PageNode *prev; // 上一个页面节点

    _s_FB_PageNode *ChildList[CHILD_NODE_COUNT_MAX]; // 子页面节点组
    uint16_t childPages_Count;                       // 该层级下的子页面数量

    uint8_t pageNmae[PAGE_NAME_SIZE_MAX]; // 页面名称
    uint32_t UID;                         // 节点唯一ID 可使用某些算法实现
    uint32_t pageLevel;                   // 当前节点层级

#ifdef SYS_IN_OS
    void *pageThreadHandle; // 线程句柄

// #ifdef FB_OS_LOCK
//     void lock(void);
//     void unlock(void);
// #endif
#endif
    // 渲染的函数 ,可用来创建线程
    void (*probe)(void *arg);        // 加载函数
    void (*draw_main)(void *arg);    // 渲染函数
    void (*remove)(void *arg);       // 退出函数
    void *(*inputSingal)(void *arg); // 事件信号
};

_s_FB_PageNode fb_root_node;

// 返回路径 最后一节目标节点句柄
//  static
//  _s_FB_PageNode * fb_node_find_of_path(uint32_t UID) //根据UID查找
_s_FB_PageNode *fb_node_find_of_path(_fb_level *level_node)
{

    uint8_t count = 0;
    // 从根目录一节节往下找
    _s_FB_PageNode *headle = &fb_root_node;

    if (level_node->levelCount - 1 == count)
    {
        return headle; // 路径节点就一个  直接返回根节点
    }

    // 根是起点，不进入查询，查询层级需要减一
    for (int i = 0; i < level_node->levelCount - 1; i++)
    {
        if (count != i)
        {
            // 在某个层级中 未找到就直接退出
            // printf("not in this level %d : %d\n",count,i);
            break;
        }
        // 每个层级 遍历该层级的子集
        for (int size = 0; size < headle->childPages_Count; size++) //
        {
            // printf("level_node->levelCount:%d headle->childPages_Count:%d  strcmp  [%s,%s]\n",
            // level_node->levelCount,
            // headle->childPages_Count,
            // level_node->levels[i+1],headle->ChildList[size]->pageNmae);
            if (strcmp(level_node->levels[i + 1], headle->ChildList[size]->pageNmae) == 0)
            {
                // 找到了，headle后移
                headle = headle->ChildList[size];
                // printf("find goal %s\n",headle->pageNmae);
                count++; // 在这个层级找到了这一页，数量标识符+1，继续进入下一页查找
                break;
            }
        }
    }

    // 看一下是否是最终要找到的哪个层级
    if (headle->pageLevel == level_node->levelCount - 1)
    {
        // printf("find this handle\n");
        return headle;
    }

    return NULL; // 未查找到
}

void rootmain(void *arg)
{
    printf("root draw\n");
}

void *rootsignal(void *arg)
{
    return _path_page_test_child;
}

int FishBone_pages_init(void)
{
    sprintf(fb_root_node.pageNmae, _name_root); // 根文件命名
    fb_root_node.draw_main = rootmain;          // 线程创建函数
    fb_root_node.inputSingal = rootsignal;
    // fb_root_node.draw_main = NULL; //根没有函数

    fb_root_node.prev = &fb_root_node; // 根节点头是本身
    fb_root_node.childPages_Count = 0; // 子页面个数为0
    // fb_root_node.ChildList[fb_root_node.childPages_Count] = &fb_root_node; // //子节点为自身
    fb_root_node.pageLevel = 0; // 意味为根页面
}

int fb_rootPages_init(fb_operations *root_ope)
{
    sprintf(fb_root_node.pageNmae, _name_root); // 根文件命名

    fb_root_node.probe = root_ope->probe;
    fb_root_node.draw_main = root_ope->draw_main; // 线程创建函数
    fb_root_node.remove = root_ope->remove;
    fb_root_node.inputSingal = root_ope->inputSingal;

    fb_root_node.prev = &fb_root_node; // 根节点头是本身
    fb_root_node.childPages_Count = 0; // 子页面个数为0
    fb_root_node.pageLevel = 0;        // 意味为根页面
}

int fb_nodeCreate(uint8_t *pageName, uint8_t *targetPath,
                  fb_probe probe_funktion, fb_drawMain funktion, fb_Singal signal, fb_remove remove)
{
    if (pageName == NULL || targetPath == NULL)
    {
        return -1;
    }

    _s_FB_PageNode *newPage = fb_malloc(sizeof(_s_FB_PageNode)); // 创建新节点
    if (newPage == NULL)
    {
        return -1;
    }

    // 名字赋值
    sprintf(newPage->pageNmae, pageName); // 名字赋值
    newPage->childPages_Count = 0;        // 子页面为0

    newPage->probe = probe_funktion;
    newPage->draw_main = funktion; // 渲染函数赋值
    newPage->inputSingal = signal; // 返回的 事件函数
    newPage->remove = remove;
    //  printf("%s is malloc find perant\n",pageName);
    // 查找父节点
    _fb_level path_node;
    parentPageProcess(targetPath, &path_node);

    // printf("%s get node\n",targetPath);
    // 得到了层级
    _s_FB_PageNode *parent = fb_node_find_of_path(&path_node);
    // printf("%x target node\n",parent);

    _fb_freeLevels(&path_node);
    if (parent == NULL)
    {
        // printf("can not find\n");
        return -1;
    }
    else
    {
        // printf("%s can find\n",targetPath);
    }

    newPage->prev = parent; // 赋值父级页面
    newPage->pageLevel = parent->pageLevel + 1;

    // 开始进行父子节点的管理
    if (parent->childPages_Count == CHILD_NODE_COUNT_MAX)
    {
        fb_free(newPage);
        return -1; // 该页面放不下了，创建失败
    }

    parent->ChildList[parent->childPages_Count] = newPage;
    parent->childPages_Count++; // 放进去后,数量+1

    // printf("%s  create\n",pageName);
    return 0; // 创建成功
}

int fb_pageCreate_ope(uint8_t *pageName, uint8_t *parentPath, fb_operations *ope)
{
    if (pageName == NULL || parentPath == NULL)
    {
        return -1;
    }
    _s_FB_PageNode *newPage = fb_malloc(sizeof(_s_FB_PageNode)); // 创建新节点
    if (newPage == NULL)
    {
        return -1;
    }
    // 名字赋值
    sprintf(newPage->pageNmae, pageName); // 名字赋值
    newPage->childPages_Count = 0;        // 子页面为0

    newPage->probe = ope->probe;
    newPage->draw_main = ope->draw_main;     // 渲染函数赋值
    newPage->inputSingal = ope->inputSingal; // 返回的 事件函数
    newPage->remove = ope->remove;
    //  printf("%s is malloc find perant\n",pageName);
    // 查找父节点
    _fb_level path_node;
    parentPageProcess(parentPath, &path_node);

    // printf("%s get node\n",targetPath);
    // 得到了层级
    _s_FB_PageNode *parent = fb_node_find_of_path(&path_node);
    // printf("%x target node\n",parent);

    _fb_freeLevels(&path_node);
    if (parent == NULL)
    {
        // printf("can not find\n");
        return -1;
    }

    // 文件重命名判断
    for (int count = 0; count < parent->childPages_Count; count++)
    {
        if (strcmp(newPage->pageNmae, parent->ChildList[count]) == 0)
        {
            fb_free(newPage);
            return -1; // 有重命名的，创建失败
        }
    }

    newPage->prev = parent;                     // 赋值父级页面
    newPage->pageLevel = parent->pageLevel + 1; // 层级在父页基础上加1

    // 开始进行父子节点的管理
    if (parent->childPages_Count == CHILD_NODE_COUNT_MAX)
    {
        fb_free(newPage);
        return -1; // 该页面放不下了，创建失败
    }

    parent->ChildList[parent->childPages_Count] = newPage;
    parent->childPages_Count++; // 放进去后,数量+1

    // printf("%s  create\n",pageName);
    return 0; // 创建成功
}

uint8_t fb_return_buffer[1024]; // 这是获取当前页面的路径
uint8_t *fb_return_page_of_path(_s_FB_PageNode *page)
{
    _s_FB_PageNode *temp = page;
    uint8_t *new_path;
    uint8_t *path = fb_return_buffer;
    if (page->pageLevel == 0)
    {
        return NULL;
    }

    for (uint8_t i = page->pageLevel; i > 0; i--) // 这是层级
    {
        new_path = prepend_parent_path(path, temp->prev->pageNmae);
        if (new_path != NULL) // 不为空
        {
            // 路径重新赋值
            strcpy(path, new_path); // 字符串赋值
            fb_free(new_path);
        }
        temp = temp->prev; // 换到上一级
    }
    // 这里最前面加上一个 /
    new_path = fb_malloc(strlen(path) + 2);
    strcpy(new_path + 1, path);
    new_path[0] = '/';
    strcpy(path, new_path);
    fb_free(new_path);

    return path; // 返回最后的path
}

uint8_t current_path[1024] = {0};
void fb_root_mangeTask(void *arg) // 用户轮转的线程
{

    uint8_t *running_path = current_path;
    _fb_level path_node;
    sprintf(running_path, "root"); // 第一页，这里是menu
    while (1)
    {
        uint8_t *temp_path = NULL;
        if (running_path == NULL)
        {
            printf("running_path = null\n");
            break;
        }
        if (parentPageProcess(running_path, &path_node)) // 路径查找
        {
            // continue;

            break;
        }
        // 查找到这个页面 获取页面句柄
        _s_FB_PageNode *headle = fb_node_find_of_path(&path_node);
        if (headle == NULL)
        {
            // 错误，销毁线程
            printf("task can not find\n");
            break;
        }
        else
        {
            _fb_freeLevels(&path_node);
        }

        if (headle->probe != NULL)
            headle->probe(arg);

        // 获得句柄
        if (headle->draw_main != NULL)
            headle->draw_main(arg); // 运行渲染函数

        if (headle->inputSingal != NULL)
        {
            temp_path = (uint8_t *)headle->inputSingal(arg);
            if (temp_path != NULL) // 返回的数据有效
            {
                strcpy(running_path, temp_path); // 阻塞等待信号返回 获取下一页目标路径或者句柄  参数是当前任务的任务句柄
            }
        }

        if (headle->remove != NULL)
            headle->remove(arg);
    }

#ifdef OSTASKDELETE
    OSTASKDELETE();
#endif
}
