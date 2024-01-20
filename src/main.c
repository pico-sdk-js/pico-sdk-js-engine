#include <stdlib.h>
#include <string.h>

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
    os_init();
    psj_repl_init();

    os_wait_for_ready();

    /* Initialize engine */
    jerry_init(JERRY_INIT_EMPTY);

    /* Register 'print' function from the extensions */
    jerryx_handler_register_global((const jerry_char_t *)"print", jerryx_handler_print);

    const jerry_char_t *script = os_flash_read();
    jerry_value_t parsed_code = jerry_parse(NULL, 0, script, strlen(script), JERRY_PARSE_STRICT_MODE);

    if (jerry_value_is_error(parsed_code))
    {
        psj_print_unhandled_exception(parsed_code);
    }
    else
    {
        jerry_value_t ret_val = jerry_run(parsed_code);
        psj_print_value(ret_val);
        jerry_release_value(ret_val);
    }

    /* Parsed source code must be freed */
    jerry_release_value(parsed_code);

    // Read in command
    while (os_get_is_running())
    {
        psj_repl_cycle();
    }

    /* Cleanup engine */
    jerry_cleanup();
    psj_repl_cleanup();
    os_cleanup();

    return 0;
}