#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>
#include <sys/types.h>

#include "jerryscript.h"
#include "lfs.h"

#define MAX_SCRIPT_LENGTH 1024

void psj_flash_init();
void psj_flash_cleanup();
int psj_flash_save(const jerry_char_t *path, const jerry_char_t *data);
int psj_flash_read(jerry_char_t *buffer, u_int32_t max_length);
int psj_flash_list(struct lfs_info *file_info, uint32_t max_file_info);

#endif // __FLASH_H
