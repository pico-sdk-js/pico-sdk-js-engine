#include "psj.h"

#include "jerryscript.h"

jerry_value_t psj_format_command(jerry_value_t request_args)
{
    jerry_value_t response;

    int err = psj_flash_reformat();
    if (err < 0)
    {
        response = psj_jerry_create_error_obj(FORMAT_ERROR);
    }
    else
    {
        response = jerry_create_null();
    }

cleanup:

    return response;
}
