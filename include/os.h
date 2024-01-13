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

#endif // __OS_H
