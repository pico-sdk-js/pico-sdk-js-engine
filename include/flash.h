#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>
#include <sys/types.h>

#include "jerryscript.h"

void psj_flash_init();
void psj_flash_cleanup();
void psj_flash_save(const jerry_char_t *jsFile);
int psj_flash_read(jerry_char_t *buffer, u_int32_t max_length);

#endif // __FLASH_H
