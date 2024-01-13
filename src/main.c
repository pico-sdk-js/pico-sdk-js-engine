#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "jerryscript.h"
#include "jerryscript-ext/handler.h"

#ifndef TARGET_NAME
#define TARGET_NAME "UNKNOWN"
#endif

#ifndef TARGET_VERSION
#define TARGET_VERSION "v0.0.0"
#endif

#define GPIO_WAIT_FOR_READY_PIN 22

#define ENDSTDIN 255
#define CR 13

void wait_for_ready()
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

static void print_value(const jerry_value_t jsvalue)
{
    jerry_value_t value;
    /* If there is an error extract the object from it */
    if (jerry_value_is_error(jsvalue))
    {
        printf("Error value detected: ");
        value = jerry_get_value_from_error(jsvalue, false);
    }
    else
    {
        value = jerry_acquire_value(jsvalue);
    }

    if (jerry_value_is_undefined(value))
    {
        printf("undefined");
    }
    else if (jerry_value_is_null(value))
    {
        printf("null");
    }
    else if (jerry_value_is_boolean(value))
    {
        if (jerry_get_boolean_value(value))
        {
            printf("true");
        }
        else
        {
            printf("false");
        }
    }
    /* Float value */
    else if (jerry_value_is_number(value))
    {
        printf("number: %lf", jerry_get_number_value(value));
    }
    /* String value */
    else if (jerry_value_is_string(value))
    {
        jerry_char_t str_buf_p[256];

        /* Determining required buffer size */
        jerry_size_t req_sz = jerry_get_string_size(value);

        if (req_sz <= 255)
        {
            jerry_string_to_char_buffer(value, str_buf_p, req_sz);
            str_buf_p[req_sz] = '\0';
            printf("%s", (const char *)str_buf_p);
        }
        else
        {
            printf("error: buffer isn't big enough");
        }
    }
    /* Object reference */
    else if (jerry_value_is_object(value))
    {
        printf("[JS object]");
    }

    printf("\n");
    jerry_release_value(value);
}

int main()
{
    bool is_done = false;
    char strg[100];
    char chr;
    int lp = 0;

    stdio_init_all();

    wait_for_ready();

    printf("%s - v%s\n", TARGET_NAME, TARGET_VERSION);

    /* Initialize engine */
    jerry_init(JERRY_INIT_EMPTY);

    /* Register 'print' function from the extensions */
    jerryx_handler_register_global((const jerry_char_t *)"print", jerryx_handler_print);

    // Read in command
    while (!is_done)
    {
        chr = getchar_timeout_us(0);
        while (chr != PICO_ERROR_TIMEOUT && chr != ENDSTDIN)
        {
            strg[lp++] = chr;
            if (chr == CR || lp == (sizeof(strg) - 1))
            {
                strg[lp] = 0; // terminate string

                if (strncmp(strg, "quit", sizeof("quit")) == 0)
                {
                    is_done = true;
                    break;
                }

                jerry_value_t ret_val;

                /* Evaluate entered command */
                ret_val = jerry_eval ((const jerry_char_t *) strg, lp, JERRY_PARSE_NO_OPTS);

                /* Print out the value */
                print_value (ret_val);

                jerry_release_value (ret_val);
                
                lp = 0; // reset string buffer pointer
                break;
            }

            chr = getchar_timeout_us(0);
        }
    }

    printf("Cleaning up..");

    /* Cleanup engine */
    jerry_cleanup();

    return 0;
}