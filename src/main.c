#include <stdlib.h>

#include "jerryscript.h"
#include "jerryscript-ext/handler.h"

#include "io.h"
#include "os.h"
#include "repl.h"

#ifndef TARGET_NAME
#define TARGET_NAME "UNKNOWN"
#endif

#ifndef TARGET_VERSION
#define TARGET_VERSION "v0.0.0"
#endif

int main()
{
    bool is_done = false;

    os_init();
    psj_repl_init();

    os_wait_for_ready();

    /* Initialize engine */
    jerry_init(JERRY_INIT_EMPTY);

    /* Register 'print' function from the extensions */
    jerryx_handler_register_global((const jerry_char_t *)"print", jerryx_handler_print);

    // Read in command
    while (!is_done)
    {
        psj_repl_cycle();
    }

    /* Cleanup engine */
    jerry_cleanup();
    psj_repl_cleanup();
    os_cleanup();

    return 0;
}