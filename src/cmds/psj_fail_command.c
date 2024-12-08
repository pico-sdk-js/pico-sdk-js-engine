#include "psj.h"
#include "jerryscript.h"

typedef int32_t (*GetNumberCallback)(int32_t x);

jerry_value_t psj_fail_command(jerry_value_t request_args)
{
    jerry_value_t value = jerry_create_object();
    GetNumberCallback fn = NULL;

    fn(3);

    return value;
}
