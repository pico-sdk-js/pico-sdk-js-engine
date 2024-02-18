#include "psj.h"

#include <stdlib.h>
#include <string.h>

#include "jerryscript.h"

jerry_value_t psj_exec_command(jerry_value_t request_args)
{
    jerry_char_t *value = psj_jerry_get_string_property(request_args, "code");
    if (value == NULL)
    {
        return psj_jerry_create_error_obj(MISSING_ARG, "code");
    }

    uint16_t value_size = strlen(value);
    jerry_value_t parse_val = jerry_parse(NULL, 0, value, value_size, JERRY_PARSE_STRICT_MODE);
    jerry_value_t ret_val;

    free(value);

    if (jerry_value_is_error(parse_val))
    {
        ret_val = parse_val;
    }
    else
    {
        ret_val = jerry_run(parse_val);
        jerry_release_value(parse_val);
    }

    if (jerry_value_is_error(ret_val))
    {
        jerry_value_t exception = ret_val;
        ret_val = psj_jerry_exception_to_error_obj(exception);
        jerry_release_value(exception);
    }
    
    return ret_val;
}

