#include "psj.h"

#include <stdlib.h>
#include <string.h>

#include "jerryscript.h"
#include "jerryscript-ext/handler.h"

void run_jerryscript_engine();

int main()
{
    os_init();

    os_wait_for_ready();

    /* Initialize engine */
    jerry_init(JERRY_INIT_EMPTY);

    psj_repl_init();
    psj_flash_init();

    run_jerryscript_engine();

cleanup:
    psj_flash_cleanup();
    psj_repl_cleanup();
    os_cleanup();

    return 0;
}

void run_jerryscript_engine()
{
    /* Register 'print' function from the extensions */
    jerryx_handler_register_global((const jerry_char_t *)"print", jerryx_handler_print);

    // Run flash program
    psj_repl_run_flash();
    
    // Read in command
    while (os_get_is_running())
    {
        psj_repl_cycle();
    }

    /* Cleanup engine */
    jerry_cleanup();
}