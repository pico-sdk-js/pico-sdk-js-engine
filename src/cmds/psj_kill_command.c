#include "psj.h"

#include "jerryscript.h"

jerry_value_t psj_kill_command(jerry_value_t request_args)
{
    return psj_jerry_create_error_obj(NOT_SUPPORTED, "kill");
}
