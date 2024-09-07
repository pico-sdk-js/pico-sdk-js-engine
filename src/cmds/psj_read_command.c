#include "psj.h"

#include <math.h>

#include "jerryscript.h"
#include "lfs.h"

jerry_value_t psj_read_command(jerry_value_t request_args)
{
    jerry_char_t *path = psj_jerry_get_string_property(request_args, "path");
    uint32_t seg = psj_jerry_get_uint32_property(request_args, "seg");
    jerry_value_t response;
    jerry_char_t *buffer = NULL;

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

    buffer = calloc(SEGMENT_SIZE, sizeof(jerry_char_t));
    int bytes_read = psj_flash_read(path, buffer, SEGMENT_SIZE, seg);
    if (bytes_read >= 0)
    {
        response = jerry_create_object();
        psj_jerry_set_uint32_property(response, "size", bytes_read);
        psj_jerry_set_uint32_property(response, "seg", seg);
        psj_jerry_set_uint32_property(response, "nSegs", (uint32_t)ceil((double)file_size/(double)SEGMENT_SIZE));
        psj_jerry_set_string_property(response, "content", buffer);
    }
    else
    {
        response = psj_jerry_create_error_obj(READ_ERROR, path);
    }

cleanup:

    if (path != NULL) free(path);

    if (buffer != NULL) free(buffer);

    return response;
}
