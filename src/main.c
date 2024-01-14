#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "jerryscript.h"
#include "jerryscript-ext/handler.h"

#include "os.h"

#ifndef TARGET_NAME
#define TARGET_NAME "UNKNOWN"
#endif

#ifndef TARGET_VERSION
#define TARGET_VERSION "v0.0.0"
#endif

#define CR 13

#define PICO_ERROR_TIMEOUT -1

#define SYNTAX_ERROR_MAX_LINE_LENGTH 256

void main_print_unhandled_exception(jerry_value_t error_value) /**< error value */
{
    assert(jerry_value_is_error(error_value));
    error_value = jerry_get_value_from_error(error_value, true);

    jerry_char_t err_str_buf[256];

    jerry_value_t err_str_val = jerry_value_to_string(error_value);
    jerry_size_t err_str_size = jerry_get_utf8_string_size(err_str_val);

    if (err_str_size >= 256)
    {
        const char msg[] = "[Error message too long]";
        err_str_size = sizeof(msg) / sizeof(char) - 1;
        memcpy(err_str_buf, msg, err_str_size + 1);
    }
    else
    {
        jerry_size_t string_end = jerry_string_to_utf8_char_buffer(err_str_val, err_str_buf, err_str_size);
        assert(string_end == err_str_size);
        err_str_buf[string_end] = 0;

        if (jerry_is_feature_enabled(JERRY_FEATURE_ERROR_MESSAGES) && jerry_get_error_type(error_value) == JERRY_ERROR_SYNTAX)
        {
            jerry_char_t *string_end_p = err_str_buf + string_end;
            unsigned int err_line = 0;
            unsigned int err_col = 0;
            char *path_str_p = NULL;
            char *path_str_end_p = NULL;

            /* 1. parse column and line information */
            for (jerry_char_t *current_p = err_str_buf; current_p < string_end_p; current_p++)
            {
                if (*current_p == '[')
                {
                    current_p++;

                    if (*current_p == '<')
                    {
                        break;
                    }

                    path_str_p = (char *)current_p;
                    while (current_p < string_end_p && *current_p != ':')
                    {
                        current_p++;
                    }

                    path_str_end_p = (char *)current_p++;

                    err_line = (unsigned int)strtol((char *)current_p, (char **)&current_p, 10);

                    current_p++;

                    err_col = (unsigned int)strtol((char *)current_p, NULL, 10);
                    break;
                }
            } /* for */

            if (err_line != 0 && err_col > 0 && err_col < SYNTAX_ERROR_MAX_LINE_LENGTH)
            {
                /* Temporarily modify the error message, so we can use the path. */
                *path_str_end_p = '\0';

                size_t source_size;
                uint8_t *source_p = jerry_port_read_source(path_str_p, &source_size);

                /* Revert the error message. */
                *path_str_end_p = ':';

                if (source_p != NULL)
                {
                    uint32_t curr_line = 1;
                    uint32_t pos = 0;

                    /* 2. seek and print */
                    while (pos < source_size && curr_line < err_line)
                    {
                        if (source_p[pos] == '\n')
                        {
                            curr_line++;
                        }

                        pos++;
                    }

                    /* Print character if:
                     * - The max line length is not reached.
                     * - The current position is valid (it is not the end of the source).
                     * - The current character is not a newline.
                     **/
                    for (uint32_t char_count = 0;
                         (char_count < SYNTAX_ERROR_MAX_LINE_LENGTH) && (pos < source_size) && (source_p[pos] != '\n');
                         char_count++, pos++)
                    {
                        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "%c", source_p[pos]);
                    }
                    jerry_port_log(JERRY_LOG_LEVEL_ERROR, "\n");

                    jerry_port_release_source(source_p);

                    while (--err_col)
                    {
                        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "~");
                    }

                    jerry_port_log(JERRY_LOG_LEVEL_ERROR, "^\n\n");
                }
            }
        }
    }

    jerry_port_log(JERRY_LOG_LEVEL_ERROR, "%s\n", err_str_buf);
    jerry_release_value(err_str_val);

    if (jerry_value_is_object(error_value))
    {
        jerry_value_t stack_str = jerry_create_string((const jerry_char_t *)"stack");
        jerry_value_t backtrace_val = jerry_get_property(error_value, stack_str);
        jerry_release_value(stack_str);

        if (jerry_value_is_array(backtrace_val))
        {
            uint32_t length = jerry_get_array_length(backtrace_val);

            /* This length should be enough. */
            if (length > 32)
            {
                length = 32;
            }

            for (uint32_t i = 0; i < length; i++)
            {
                jerry_value_t item_val = jerry_get_property_by_index(backtrace_val, i);

                if (jerry_value_is_string(item_val))
                {
                    jerry_size_t str_size = jerry_get_utf8_string_size(item_val);

                    if (str_size >= 256)
                    {
                        printf("%6u: [Backtrace string too long]\n", i);
                    }
                    else
                    {
                        jerry_size_t string_end = jerry_string_to_utf8_char_buffer(item_val, err_str_buf, str_size);
                        assert(string_end == str_size);
                        err_str_buf[string_end] = 0;

                        printf("%6u: %s\n", i, err_str_buf);
                    }
                }

                jerry_release_value(item_val);
            }
        }

        jerry_release_value(backtrace_val);
    }

    jerry_release_value(error_value);
} /* main_print_unhandled_exception */

static void print_value(const jerry_value_t jsvalue)
{
    /* If there is an error extract the object from it */
    if (jerry_value_is_error(jsvalue))
    {
        main_print_unhandled_exception(jsvalue);
        return;
    }

    if (jerry_value_is_undefined(jsvalue))
    {
        printf("undefined");
    }
    else if (jerry_value_is_null(jsvalue))
    {
        printf("null");
    }
    else if (jerry_value_is_boolean(jsvalue))
    {
        if (jerry_get_boolean_value(jsvalue))
        {
            printf("true");
        }
        else
        {
            printf("false");
        }
    }
    /* Float value */
    else if (jerry_value_is_number(jsvalue))
    {
        printf("number: %lf", jerry_get_number_value(jsvalue));
    }
    /* String value */
    else if (jerry_value_is_string(jsvalue))
    {
        jerry_char_t str_buf_p[256];

        /* Determining required buffer size */
        jerry_size_t req_sz = jerry_get_string_size(jsvalue);

        if (req_sz <= 255)
        {
            jerry_string_to_char_buffer(jsvalue, str_buf_p, req_sz);
            str_buf_p[req_sz] = '\0';
            printf("%s", (const char *)str_buf_p);
        }
        else
        {
            printf("error: buffer isn't big enough");
        }
    }
    /* Object reference */
    else if (jerry_value_is_object(jsvalue))
    {
        jerry_value_t stringifiedObj = jerry_json_stringify(jsvalue);
        print_value(stringifiedObj);
        jerry_release_value(stringifiedObj);
    }

    printf("\n");
}

int main()
{
    bool is_done = false;
    char strg[100];
    char chr;
    int lp = 0;

    os_init();

    os_wait_for_ready();

    printf("%s - v%s\n", TARGET_NAME, TARGET_VERSION);

    /* Initialize engine */
    jerry_init(JERRY_INIT_EMPTY);

    /* Register 'print' function from the extensions */
    jerryx_handler_register_global((const jerry_char_t *)"print", jerryx_handler_print);

    // Read in command
    while (!is_done)
    {
        chr = os_getchar_timeout_us(0);
        while (os_getchar_timeout_us_is_valid(chr))
        {
            strg[lp] = chr;
            if (chr == CR || lp == (sizeof(strg) - 1))
            {
                strg[lp] = 0; // terminate string

                if (strcmp(strg, "quit") == 0)
                {
                    is_done = true;
                    break;
                }

                jerry_value_t parse_val = jerry_parse(NULL, 0, (const jerry_char_t *)strg, lp, JERRY_PARSE_STRICT_MODE);

                if (jerry_value_is_error(parse_val))
                {
                    main_print_unhandled_exception(parse_val);
                }
                else
                {
                    jerry_value_t ret_val = jerry_run(parse_val);
                    print_value(ret_val);
                    jerry_release_value(ret_val);
                }

                jerry_release_value(parse_val);
                lp = 0; // reset string buffer pointer

                break;
            }
            else
            {
                lp++;
            }

            chr = os_getchar_timeout_us(0);
        }
    }

    printf("Cleaning up...\n");

    /* Cleanup engine */
    jerry_cleanup();

    os_cleanup();

    return 0;
}