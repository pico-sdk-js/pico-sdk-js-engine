#include "psj.h"

#include <stdlib.h>
#include <string.h>

#include "modules.h"
#include "jerryscript.h"

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

        init_configuration();

        psj_run_js_loop();

        cleanup_configuration();

        psj_flash_cleanup();
        psj_repl_cleanup();

        reset_modules();

        jerry_cleanup();

     } while (os_get_is_running());

    os_cleanup();

    return 0;
}
