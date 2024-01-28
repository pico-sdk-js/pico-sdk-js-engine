#include "os.h"

#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#include <stdlib.h>
#include <ctype.h>

#define GPIO_WAIT_FOR_READY_PIN 22
#define ENDSTDIN 255

const uint8_t *flash_buffer = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

void os_init()
{
    stdio_init_all();
}

void os_cleanup()
{
}

void os_wait_for_ready()
{
    gpio_init(GPIO_WAIT_FOR_READY_PIN);
    gpio_set_dir(GPIO_WAIT_FOR_READY_PIN, GPIO_IN);
    gpio_pull_up(GPIO_WAIT_FOR_READY_PIN);

    // Wait for gpio to initialize
    while (gpio_get(GPIO_WAIT_FOR_READY_PIN) == 0)
    {
        printf("#");
        sleep_ms(100);
    }

    while (gpio_get(GPIO_WAIT_FOR_READY_PIN))
    {
        printf(".");
        sleep_ms(1000);
    }

    printf("\n");
    gpio_deinit(GPIO_WAIT_FOR_READY_PIN);
}

void os_reset_usb_boot(uint32_t usb_activity_gpio_pin_mask, uint32_t disable_interface_mask)
{
    reset_usb_boot(usb_activity_gpio_pin_mask, disable_interface_mask);
}

int os_getchar_timeout_us(uint32_t timeout)
{
    return getchar_timeout_us(timeout);
}

bool os_getchar_timeout_us_is_valid(int chr)
{
    return chr != PICO_ERROR_TIMEOUT && chr != ENDSTDIN;
}

bool os_get_is_running()
{
    return true;
}

void os_exit()
{
}

void os_process_input(char c, char *s, int max_length, int *sp)
{
    if (isprint(c) && *sp < max_length - 1)
    {
        s[*sp] = c;
        (*sp) += 1;
    }
}

void os_flash_range_erase(uint32_t flash_offs, size_t count)
{
    flash_range_erase(flash_offs, count);
}

void os_flash_range_program(uint32_t flash_offs, const uint8_t *data, size_t count)
{
    flash_range_program(flash_offs, data, count);
}

uint8_t *os_get_flash_buffer()
{
    return (uint8_t*)flash_buffer;
}

uint32_t os_save_and_disable_interrupts()
{
    return save_and_disable_interrupts();
}

void os_restore_interrupts(uint32_t status)
{
    restore_interrupts(status);
}
