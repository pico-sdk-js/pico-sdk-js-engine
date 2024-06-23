#include "psj.h"

#include <stdlib.h>
#include <string.h>

#include "modules.h"
#include "jerryscript.h"

void run_repl_engine();

int main(int argc, char *argv[])
{
    os_set_is_running(true);
    os_init(argc, argv);

    do 
    {
        /* Initialize engine */
        jerry_init(JERRY_INIT_EMPTY);

        init_modules();

        psj_repl_init();
        psj_flash_init();

        init_core1();

        start_core1();
        
        run_repl_engine();

        cleanup_core1();

        psj_flash_cleanup();
        psj_repl_cleanup();

        reset_modules();

     } while (os_get_is_running());

    os_cleanup();

    return 0;
}

void run_repl_engine()
{
    os_set_is_repl_running(true);

    // Read in command
    while (os_get_is_repl_running())
    {
        psj_repl_cycle();
    }

    /* Cleanup engine */
    jerry_cleanup();
}