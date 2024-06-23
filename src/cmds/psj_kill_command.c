#include "psj.h"

#include "jerryscript.h"

jerry_value_t psj_kill_command(jerry_value_t request_args)
{
    kill_core1();

cleanup:

    return jerry_create_null();
}
