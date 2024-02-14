#include "psj.h"
#include "jerryscript.h"

jerry_value_t psj_stats_command(jerry_value_t request_args)
{
    jerry_value_t value = jerry_create_object();
    psj_jerry_set_string_property(value, "name", TARGET_NAME);
    psj_jerry_set_string_property(value, "version", TARGET_VERSION);
    return value;
}
