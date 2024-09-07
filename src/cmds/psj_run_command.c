#include "psj.h"

#include <stdlib.h>
#include <string.h>

#include "jerryscript.h"

jerry_value_t psj_run_command(jerry_value_t request_args)
{
    jerry_char_t *path = psj_jerry_get_string_property(request_args, "path");
    jerry_value_t response = jerry_create_undefined();

    if (path == NULL)
    {
        response = psj_jerry_create_error_obj(MISSING_ARG, "path");
        goto cleanup;
    }

    uint32_t file_size;
    if (psj_flash_file_size(path, &file_size) < 0)
    {
        response = psj_jerry_create_error_obj(FILE_NOT_FOUND, path);
        goto cleanup;
    }

    // Set next resource to execute
    psj_set_next_resource(path);

cleanup:

    if (path != NULL) free(path);

    return response;
}