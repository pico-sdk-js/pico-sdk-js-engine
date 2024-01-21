#include <libgen.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>

#include "jerryscript-port.h"

void jerry_port_fatal(jerry_fatal_code_t code)
{
    exit(code);
}

void jerry_port_log(jerry_log_level_t level, const char *format, ...)
{
    switch (level)
    {
    case JERRY_LOG_LEVEL_ERROR:
        fprintf(stderr, "\033[1;31mERR: \033[0m");
        break;
    case JERRY_LOG_LEVEL_WARNING:
        fprintf(stderr, "\033[0;33mWRN: \033[0m");
        break;
    case JERRY_LOG_LEVEL_DEBUG:
        fprintf(stderr, "\033[0;32mDBG: \033[0m");
        break;
    case JERRY_LOG_LEVEL_TRACE:
        fprintf(stderr, "\033[0;34mTRC: \033[0m");
        break;
    default:
        break;
    }

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void jerry_port_print_char(char c)
{
    putchar(c);
}

double jerry_port_get_local_time_zone_adjustment(double unix_ms, bool is_utc)
{
    struct tm tm;
    time_t now = (time_t)(unix_ms / 1000);
    localtime_r(&now, &tm);
    if (!is_utc)
    {
        now -= tm.tm_gmtoff;
        localtime_r(&now, &tm);
    }
    return ((double)tm.tm_gmtoff) * 1000;
}

double jerry_port_get_current_time(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
    {
        return 0;
    }

    return ((double)tv.tv_sec) * 1000.0 + ((double)tv.tv_usec) / 1000.0;
}

static jerry_context_t *current_context_p = NULL;

void jerry_port_default_set_current_context(jerry_context_t *context_p)
{
    current_context_p = context_p;
}

jerry_context_t *jerry_port_get_current_context(void)
{
    return current_context_p;
}

uint8_t *jerry_port_read_source(const char *file_name_p, size_t *out_size_p)
{
    return NULL;
}

/**
 * Release the previously opened file's content.
 */
void jerry_port_release_source(uint8_t *buffer_p) /**< buffer to free */
{
    free(buffer_p);
}

jerry_value_t jerry_port_module_resolve(const jerry_value_t specifier, const jerry_value_t referrer, void *user_p)
{
    // Resolves a module using the specifier string. If a referrer is a module,
    // and specifier is a relative path, the base path should be the directory
    // part extracted from the path of the referrer module.

    // The callback function of jerry_module_link may call this function
    // if it cannot resolve a module. Furthermore if the callback is NULL,
    // this function is used for resolving modules.

    // The default implementation only resolves ECMAScript modules, and does
    // not (currently) use the user data.
    jerry_size_t req_sz = jerry_get_utf8_string_size(specifier);
    JERRY_VLA(jerry_char_t, str_buf_p, req_sz + 1);
    jerry_string_to_utf8_char_buffer(specifier, str_buf_p, req_sz);
    str_buf_p[req_sz] = '\0';

    jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "Module Resolution '%s'\n", str_buf_p);
    return jerry_create_undefined();
}

void jerry_port_module_release(const jerry_value_t realm)
{
    // This function releases the known modules, forcing their reload
    // when resolved again later. The released modules can be filtered
    // by realms. This function is only called by user applications.
}

size_t jerry_port_normalize_path(const char *in_path_p, char *out_buf_p, size_t out_buf_size, char *base_file_p)
{
    size_t ret = 0;

    char *base_dir_p = dirname(base_file_p);
    const size_t base_dir_len = strnlen(base_dir_p, PATH_MAX);
    const size_t in_path_len = strnlen(in_path_p, PATH_MAX);
    char *path_p = (char *)malloc(base_dir_len + 1 + in_path_len + 1);

    memcpy(path_p, base_dir_p, base_dir_len);
    memcpy(path_p + base_dir_len, "/", 1);
    memcpy(path_p + base_dir_len + 1, in_path_p, in_path_len + 1);

    char *norm_p = realpath(path_p, NULL);
    free(path_p);

    if (norm_p != NULL)
    {
        const size_t norm_len = strnlen(norm_p, out_buf_size);
        if (norm_len < out_buf_size)
        {
            memcpy(out_buf_p, norm_p, norm_len + 1);
            ret = norm_len;
        }

        free(norm_p);
    }

    return ret;
}

jerry_value_t jerry_port_get_native_module(jerry_value_t name)
{
    jerry_size_t req_sz = jerry_get_utf8_string_size(name);
    JERRY_VLA(jerry_char_t, str_buf_p, req_sz + 1);
    jerry_string_to_utf8_char_buffer(name, str_buf_p, req_sz);
    str_buf_p[req_sz] = '\0';

    jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "Native Module Resolution '%s'\n", str_buf_p);
    return jerry_create_undefined();
}

void jerry_port_track_promise_rejection(const jerry_value_t promise, const jerry_promise_rejection_operation_t operation)
{
    (void)operation; /* unused */

    jerry_value_t reason = jerry_get_promise_result(promise);
    jerry_value_t reason_to_string = jerry_value_to_string(reason);
    jerry_size_t req_sz = jerry_get_utf8_string_size(reason_to_string);
    JERRY_VLA(jerry_char_t, str_buf_p, req_sz + 1);
    jerry_string_to_utf8_char_buffer(reason_to_string, str_buf_p, req_sz);
    str_buf_p[req_sz] = '\0';

    jerry_release_value(reason_to_string);
    jerry_release_value(reason);

    jerry_port_log(JERRY_LOG_LEVEL_WARNING, "Uncaught (in promise) %s\n", str_buf_p);
}