#include "psj.h"

#include <stdlib.h>
#include <string.h>

#include "modules.h"
#include "jerryscript.h"

jerry_value_t psj_reset_command(jerry_value_t request_args)
{
    // clear\reset existing modules
    reset_modules();

    // Cleanup existing jerry context
    jerry_cleanup();

    // Re-initialize Jerryscript
    jerry_init(JERRY_INIT_EMPTY);

    // Re-setup modules
    init_modules();

    return jerry_create_undefined();
}

