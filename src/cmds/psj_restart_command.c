#include "psj.h"

#include "jerryscript.h"

jerry_value_t psj_restart_command(jerry_value_t request_args)
{
    uint8_t hardReset = psj_jerry_get_uint32_property(request_args, "hard");

    os_restart(hardReset != 0);

    return jerry_create_undefined();
}

