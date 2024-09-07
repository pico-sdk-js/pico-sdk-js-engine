#include "psj.h"

#include "jerryscript.h"

jerry_value_t psj_kill_command(jerry_value_t request_args)
{
    psj_set_next_resource(NULL);

    return jerry_create_undefined();
}
