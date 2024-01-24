#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "jerryscript-port.h"

const int prefix_len = 16;
const char err_prefix[] = "\033[1;31mERR: \033[0m";
const char wrn_prefix[] = "\033[0;33mWRN: \033[0m";
const char dbg_prefix[] = "\033[0;32mDBG: \033[0m";
const char trc_prefix[] = "\033[0;34mTRC: \033[0m";

void jerry_port_fatal(jerry_fatal_code_t code)
{
    exit(code);
}

void jerry_port_log(jerry_log_level_t level, const char *format, ...)
{
    char *logfmt = malloc((strlen(format) + prefix_len) * sizeof(char));
    logfmt[0] = 0;

    switch (level)
    {
    case JERRY_LOG_LEVEL_ERROR:
        strcpy(logfmt, err_prefix);
        break;
    case JERRY_LOG_LEVEL_WARNING:
        strcpy(logfmt, wrn_prefix);
        break;
    case JERRY_LOG_LEVEL_DEBUG:
        strcpy(logfmt, dbg_prefix);
        break;
    case JERRY_LOG_LEVEL_TRACE:
        strcpy(logfmt, trc_prefix);
        break;
    default:
        break;
    }

    strcat(logfmt, format);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, logfmt, args);
    va_end(args);

    free(logfmt);
}

void jerry_port_print_char(char c)
{
    putchar(c);
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