#include "psj.h"

#include <math.h>

#include "jerryscript.h"
#include "lfs.h"

jerry_value_t psj_config_read_command(jerry_value_t request_args)
{
    jerry_char_t *name = psj_jerry_get_string_property(request_args, "name");
    jerry_char_t *strValue = NULL;
    jerry_value_t response;

    if (name == NULL || name[0] == '\0')
    {
        response = psj_jerry_create_error_obj(MISSING_ARG, "name");
        goto cleanup;
    }

    if (strncmp(name, "autorun", 8) == 0)
    {
        strValue = config_get_autorun();
        if (strValue != NULL)
        {
            response = jerry_create_string(strValue);
        }
        else
        {
            response = jerry_create_null();
        }
    }
    else
    {
        response = psj_jerry_create_error_obj(UNKNOWN_CONFIG, name);
    }

cleanup:

    if (name != NULL) free(name);
    if (strValue != NULL) free(strValue);

    return response;
}
