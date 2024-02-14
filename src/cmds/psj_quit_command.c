#include "psj.h"
#include "jerryscript.h"

jerry_value_t psj_quit_command(jerry_value_t request_args)
{
    os_exit();

    return jerry_create_null();
}
