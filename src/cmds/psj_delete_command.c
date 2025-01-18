#include "psj.h"

#include "jerryscript.h"

jerry_value_t psj_delete_command(jerry_value_t request_args)
{
    jerry_char_t *path = psj_jerry_get_string_property(request_args, "path");
    jerry_value_t response;

    if (path == NULL || path[0] == '\0')
    {
        response = psj_jerry_create_error_obj(MISSING_ARG, "path");
        goto cleanup;
    }

    if (psj_is_hidden_file(path))
    {
        response = psj_jerry_create_error_obj(HIDDEN_PATH);
        goto cleanup;
    }

    int err = psj_flash_delete(path);
    if (err == 0)
    {
        response = psj_jerry_create_error_obj(FILE_NOT_FOUND, path);
    }
    else if (err < 0)
    {
        response = psj_jerry_create_error_obj(DELETE_ERROR, path);
    }
    else
    {
        response = jerry_create_null();
    }

cleanup:

    if (path != NULL) free(path);

    return response;
}
