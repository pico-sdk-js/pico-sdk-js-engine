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

        run_repl_engine();

        psj_flash_cleanup();
        psj_repl_cleanup();

        reset_modules();

     } while (os_get_is_running());

    os_cleanup();

    return 0;
}

static jerry_value_t vm_exec_stop_callback (void *user_p)
{
    if (!os_get_is_repl_running()) {
        return jerry_create_string("Script aborted");
    }

    psj_repl_cycle();

    return jerry_create_undefined();
}

void run_repl_engine()
{
    os_set_is_repl_running(true);

    psj_repl_run_resource(NULL, vm_exec_stop_callback);

    /* Cleanup engine */
    jerry_cleanup();
}