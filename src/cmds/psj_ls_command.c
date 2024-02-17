#include "psj.h"
#include "jerryscript.h"
#include "lfs.h"

jerry_value_t psj_ls_command(jerry_value_t request_args)
{
    struct lfs_info file_info[16];

    int file_count = psj_flash_list(file_info, 16);

    jerry_value_t file_array = jerry_create_array(file_count);

    for (int idx = 0; idx < file_count; idx++)
    {
        jerry_value_t file_object = jerry_create_object();
        psj_jerry_set_string_property(file_object, "name", file_info[idx].name);
        psj_jerry_set_uint32_property(file_object, "size", file_info[idx].size);
        jerry_set_property_by_index (file_array, idx, file_object);
        jerry_release_value(file_object);
    }

    return file_array;
}
