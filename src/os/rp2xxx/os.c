#include "os.h"

#include "pico/stdlib.h"
#include <ctype.h>

#define GPIO_WAIT_FOR_READY_PIN 22
#define ENDSTDIN 255

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

    while (gpio_get(GPIO_WAIT_FOR_READY_PIN))
    {
        printf(".");
        sleep_ms(1000);
    }

    printf("\n");
    gpio_deinit(GPIO_WAIT_FOR_READY_PIN);
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