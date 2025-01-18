#include "psj.h"

#include <math.h>

#include "jerryscript.h"
#include "lfs.h"

jerry_value_t psj_config_write_command(jerry_value_t request_args)
{
    jerry_char_t *name = psj_jerry_get_string_property(request_args, "name");
    jerry_value_t value = psj_jerry_get_property(request_args, "value");
    jerry_value_t response;
    jerry_char_t *strValue = NULL;

    if (name == NULL || name[0] == '\0')
    {
        response = psj_jerry_create_error_obj(MISSING_ARG, "name");
        goto cleanup;
    }

    if (strncmp(name, "autorun", 8) == 0)
    {
        if (jerry_value_is_string(value) || jerry_value_is_null(value))
        {
            strValue = psj_jerry_to_string(value);
            config_set_autorun(strValue);
            response = jerry_create_null();
        }
        else
        {
            response = psj_jerry_create_error_obj(INVALID_CONFIG_VALUE, name, "string");
        }
    }
    else
    {
        response = psj_jerry_create_error_obj(UNKNOWN_CONFIG, name);
    }

cleanup:

    if (name != NULL) free(name);

    if (value != 0) jerry_release_value(value);
    if (strValue != NULL) free(strValue);

    return response;
}
