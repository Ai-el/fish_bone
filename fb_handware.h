#ifndef __FB_HANDWARE_H_
#define __FB_HANDWARE_H_
#include "string.h"
#include "stdio.h"
#include <stdint.h>


typedef struct _fb_level_s{

    uint32_t levelCount;
    uint8_t** levels; //字符，页面名

}_fb_level;

uint8_t parentPageProcess(const uint8_t* path,_fb_level* level);
char* prepend_parent_path(const char* current_path, const char* parent_folder);

void freeLevels(char **levels, int levelCount);
void _fb_freeLevels(_fb_level* level_s);

#endif