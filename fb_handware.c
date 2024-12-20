
#include "fb_handware.h"
#include "fishBone_Struct.h"
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 解析路径字符串，返回路径层级
char **parsePath(const char *path, int *levelCount) {
    if (path == NULL || levelCount == NULL) {
        return NULL;
    }

    // 计算路径层级数量
    int count = 1; // 至少有一个层级（根目录）
    for (const char *p = path; *p != '\0'; p++) {
        if (*p == '/') {
            count++;
        }
    }

    // 分配内存
    char **levels = (char **)malloc(count * sizeof(char *));
    if (levels == NULL) {
        return NULL;
    }

    // 分割路径
    const char *start = path;
    int level = 0;
    for (const char *p = path; ; p++) {
        if (*p == '/' || *p == '\0') {
            int length = p - start;
            if (length > 0) {
                levels[level] = (char *)malloc(length + 1);
                if (levels[level] == NULL) {
                    // 内存分配失败，清理已分配的内存
                    for (int i = 0; i < level; i++) {
                        free(levels[i]);
                    }
                    free(levels);
                    return NULL;
                }
                strncpy(levels[level], start, length);
                levels[level][length] = '\0';
                level++;
            }
            start = p + 1;
            if (*p == '\0') {
                break;
            }
        }
    }

    *levelCount = count;
    return levels;
}

// 打印路径层级
void printLevels(char **levels, int levelCount) {
    for (int i = 0; i < levelCount; i++) {
        printf("Level %d: %s\n", i, levels[i]);
    }
}

// 释放内存
void freeLevels(char **levels, int levelCount) {
    for (int i = 0; i < levelCount; i++) {
        free(levels[i]);
    }
    fb_free(levels);
    
}

void _fb_freeLevels(_fb_level * level_s) {
    for (int i = 0; i < level_s->levelCount; i++) {
        fb_free(level_s->levels[i]);
    }
    fb_free(level_s->levels);
}

int tttmain() {
    const char *path = "/";
    int levelCount;
    
    char **levels = parsePath(path, &levelCount);
    if (levels == NULL) {
        fprintf(stderr, "Failed to parse path\n");
        return 1;
    }

    printLevels(levels, levelCount);
    freeLevels(levels, levelCount);

    return 0;
}


uint8_t parentPageProcess(const uint8_t* path,_fb_level* level)
{
    level->levels = parsePath(path, &level->levelCount);
    if (level->levels == NULL) {
        // fprintf(stderr, "Failed to parse path\n");
        printf("Failed to parse path\n");
        return 1;
    }
    // printf("levelCount = %d\n",level->levelCount);
    // printLevels(level->levels, level->levelCount); 
    return 0;
    //得到了这个路径下的所有路径的层级和对应的字符
}



//往后添加路径
char* append_filename_to_path(const char* directory, const char* filename) {
    // 计算所需的缓冲区大小
    size_t dir_len = strlen(directory);
    size_t file_len = strlen(filename);
    size_t buffer_size = dir_len + file_len + 2; // 额外的1个字节用于'/'，另一个用于'\0'

    // 分配内存
    char* full_path = (char*)malloc(buffer_size);
    if (full_path == NULL) {
        return NULL; // 内存分配失败
    }

    // 复制目录
    strcpy(full_path, directory);

    // 添加路径分隔符（如果需要）
    if (directory[dir_len - 1] != '/') {
        strcat(full_path, "/");
    }

    // 添加文件名
    strcat(full_path, filename);

    return full_path;
}

// 往前倒插路径
char* prepend_parent_path(const char* current_path, const char* parent_folder) {
    // 计算所需的缓冲区大小
    size_t parent_len = strlen(parent_folder);
    size_t current_len = strlen(current_path);
    size_t buffer_size = parent_len + current_len + 2; // 额外的1个字节用于路径分隔符，另一个用于'\0'

    // 分配内存
    char* new_path = (char*)malloc(buffer_size);
    if (new_path == NULL) {
        return NULL; // 内存分配失败
    }

    // 构建新的路径
    strcpy(new_path, parent_folder); // 复制父文件夹名
    strcat(new_path, "/"); // 添加路径分隔符
    strcat(new_path, current_path); // 添加当前文件名

    return new_path;
}

