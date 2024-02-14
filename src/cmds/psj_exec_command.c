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

    free(value);

    if (jerry_value_is_error(parse_val))
    {
        // Return & don't release the parse error
        return parse_val;
    }
    else
    {
        jerry_value_t ret_val = jerry_run(parse_val);
        jerry_release_value(parse_val);
        return ret_val;
    }
}

