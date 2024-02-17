#include "psj.h"
#include "jerryscript.h"
#include "lfs.h"

jerry_value_t psj_write_command(jerry_value_t request_args)
{
    jerry_char_t *path = psj_jerry_get_string_property(request_args, "path");
    jerry_char_t *content = psj_jerry_get_string_property(request_args, "content");
    jerry_value_t response;

    if (path == NULL)
    {
        response = psj_jerry_create_error_obj(MISSING_ARG, "path");
        goto cleanup;
    }

    if (content == NULL)
    {
        response = psj_jerry_create_error_obj(MISSING_ARG, "content");
        goto cleanup;
    }

    int bytes_written = psj_flash_save(path, content);
    if (bytes_written >= 0)
    {
        response = jerry_create_object();
        psj_jerry_set_uint32_property(response, "bytes", bytes_written);
    }
    else
    {
        response = psj_jerry_create_error_obj(WRITE_ERROR, path);
    }

cleanup:

    if (path != NULL) free(path);
    if (content != NULL) free(content);

    return response;
}
