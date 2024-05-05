#ifndef __OS_H
#define __OS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

void os_init(int argc, char *argv[]);
void os_cleanup();

void os_wait_for_ready();
int os_getchar_timeout_us(uint32_t timeout);
bool os_getchar_timeout_us_is_valid(int chr);

bool os_get_is_running();
void os_exit();

void os_process_input(char c, char *s, int max_length, int *position);

// Flash functions
/*
Flash ROM is 2MB in total starting at XIP_BASE and includes this program's code
| ----- 1MB ----- | ----- 1MB ----- |
| this prog       | User's js       |
0 ....... 0F FFFF |10 0000 . 1F FFFF|
*/

// Flash start is defined in XIP_BASE
// Flash JS start is defined as XIP_BASE+1MB

// Available flash memory starts at 1MB to allow room for this process
#define FLASH_TARGET_OFFSET (1024 * 1024)
#define FLASH_TARGET_SIZE (1024 * 1024)

uint32_t os_get_flash_buffer_size();
void os_flash_init();
void os_flash_cleanup();
void os_flash_save(char *jsFile);
char *os_flash_read();

void os_reset_usb_boot(uint32_t usb_activity_gpio_pin_mask, uint32_t disable_interface_mask);

void os_flash_range_erase(uint32_t flash_offs, size_t count);
void os_flash_range_program(uint32_t flash_offs, const uint8_t *data, size_t count);
uint8_t *os_get_flash_buffer();

uint32_t os_save_and_disable_interrupts();
void os_restore_interrupts(uint32_t status);

#endif // __OS_H
