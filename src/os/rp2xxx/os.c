#include "os.h"

#include "pico/stdlib.h"

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