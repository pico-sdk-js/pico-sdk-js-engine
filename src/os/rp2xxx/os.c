#include "psj.h"

#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "hardware/watchdog.h"

#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>

#define GPIO_WAIT_FOR_READY_PIN 22
#define ENDSTDIN 255

const uint8_t *flash_buffer = (const uint8_t *)(XIP_NOCACHE_NOALLOC_BASE + FLASH_TARGET_OFFSET);

void os_init(int argc, char *argv[])
{
    stdio_init_all();
}

void os_cleanup()
{
}

void os_restart(bool hard)
{
    jsengine_set_state(JSENGINE_STATE_REQ_STOPPING);

    if (hard)
    {
        os_set_is_running(false);
        watchdog_reboot(0, 0, 3000);
    }
}

void os_exit()
{
    // do nothing when client notifies of exit
}

uint32_t os_get_total_ram()
{
    extern char __StackLimit, __bss_end__;

    return &__StackLimit - &__bss_end__;
}

uint32_t os_get_used_ram()
{
    struct mallinfo m = mallinfo();

    return m.uordblks;
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
    if (multicore_lockout_victim_is_initialized(1))
    {
        multicore_lockout_start_blocking();
    }

    return save_and_disable_interrupts();
}

void os_restore_interrupts(uint32_t status)
{
    restore_interrupts(status);

    if (multicore_lockout_victim_is_initialized(1))
    {
        multicore_lockout_end_blocking();    
    }
}

// Define weak functions to handle ISR_HARDFAULT
void isr_hardfault(void)
{
    printf("!#isr_hardfault#!\n");
    fflush(stdout);
    
    watchdog_reboot(0, 0, 0);
}
