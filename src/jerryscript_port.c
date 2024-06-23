#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "utstring.h"
#include "utlist.h"

#include "psj.h"

typedef struct __log_msg_node {
    jerry_value_t response;
    struct __log_msg_node *prev;
    struct __log_msg_node *next;
} log_msg_node;

log_msg_node *msg_head = NULL;

void jerry_port_fatal(jerry_fatal_code_t code)
{
    exit(code);
}

void psj_jerry_port_log_flush()
{
    log_msg_node *node;

    if (is_interrupts_suspended())
    {
        return;
    }

    while (msg_head != NULL)
    {
        node = msg_head;

        DL_DELETE(msg_head, node);
        jerry_char_t *jsonValue = psj_jerry_stringify(node->response);
        printf("%s\n", jsonValue);

        jerry_release_value(node->response);
        free(node);
        free(jsonValue);
    }

    fflush(stdout);
}

void jerry_port_log(jerry_log_level_t level, const char *format, ...)
{
    jerry_value_t response = jerry_create_object();
    psj_jerry_set_string_property(response, "cmd", "log");

    jerry_value_t value = jerry_create_object();
    psj_jerry_set_uint32_property(value, "level", level);

    psj_jerry_set_uint32_property(value, "core", get_core_number());

    va_list args;
    va_start(args, format);
    jerry_char_t *msg = VS(format, args);
    psj_jerry_set_string_property(value, "msg", msg);
    va_end(args);

    psj_jerry_set_property(response, "value", value);

    log_msg_node *node = malloc(sizeof(log_msg_node));
    node->response = response;
    DL_APPEND(msg_head, node);

    psj_jerry_port_log_flush();

    free(msg);
    jerry_release_value(value);
}

UT_string *output_buffer = NULL;

char *psj_escape_char(char c)
{
    switch (c)
    {
        case '%':
            return S("%%");

        case '\\':
            return S("\\\\");

        case '"':
            return S("\\\"");

        case '\'':
            return S("\\\'");
    
    default:
        {
            char *newString = malloc(2*sizeof(char));
            newString[0] = c;
            newString[1] = 0;
            return newString;
        }
    }
}

void jerry_port_print_char(char c)
{
    if (output_buffer == NULL)
    {
        utstring_new(output_buffer);
    }

    if (c == '\n')
    {
        jerry_port_log(JERRY_LOG_LEVEL_TRACE+1, utstring_body(output_buffer));
        utstring_renew(output_buffer);
    }
    else
    {
        char *new_str = psj_escape_char(c);
        utstring_bincpy(output_buffer, new_str, strlen(new_str));
    }
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
    jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "Read source '%s'", file_name_p);
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

    jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "Module Resolution '%s'", str_buf_p);
    return jerry_create_undefined();
}

void jerry_port_module_release(const jerry_value_t realm)
{
    // This function releases the known modules, forcing their reload
    // when resolved again later. The released modules can be filtered
    // by realms. This function is only called by user applications.
    jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "Native Module Release");
}

jerry_value_t jerry_port_get_native_module(jerry_value_t name)
{
    jerry_char_t *str_name = psj_jerry_to_string(name);

    jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "Native Module Resolution '%s'", str_name);

    jerry_value_t module = get_module(str_name);

    free(str_name);

    return module;
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

    jerry_port_log(JERRY_LOG_LEVEL_WARNING, "Uncaught (in promise) %s", str_buf_p);
}