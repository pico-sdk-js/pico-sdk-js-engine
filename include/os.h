#ifndef __OS_H
#define __OS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

void os_init();
void os_cleanup();

void os_wait_for_ready();
int os_getchar_timeout_us(uint32_t timeout);
bool os_getchar_timeout_us_is_valid(int chr);

bool os_get_is_running();
void os_exit();

void os_process_input(char c, char* s, int max_length, int* position);

// Flash functions
/*
Flash ROM is 2MB in total starting at XIP_BASE and includes this program's code
| ----- 1MB ----- | ----- 1MB ----- |
| this prog       | User's js       |
0 ....... 0F FFFF |10 0000 . 1F FFFF|
*/

// Flash start is defined in XIP_BASE
// Flash JS start is defined as XIP_BASE+1MB

uint32_t os_get_flash_buffer_size();
void os_flash_init();
void os_flash_cleanup();
void os_flash_save(char *jsFile);
char* os_flash_read();

#endif // __OS_H
