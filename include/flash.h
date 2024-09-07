#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>
#include <sys/types.h>

#include "jerryscript.h"
#include "lfs.h"

// One segment is 256 bytes
#define SEGMENT_SIZE (256)

void psj_flash_init();
int psj_flash_reformat();
void psj_flash_cleanup();
int psj_flash_file_size(const jerry_char_t *path, uint32_t *size);
int psj_flash_save(const jerry_char_t *path, const jerry_char_t *data, const bool append);
int psj_flash_read_all(const jerry_char_t *path, jerry_char_t *buffer, uint32_t max_length);
int psj_flash_read(const jerry_char_t *path, jerry_char_t *buffer, uint32_t max_length, uint32_t segment);
int psj_flash_list(struct lfs_info *file_info, uint32_t max_file_info);
int psj_flash_delete(const jerry_char_t *path);

#endif // __FLASH_H
