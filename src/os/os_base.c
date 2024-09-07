#include "os.h"

#include <stdbool.h>

bool is_running = true;
bool is_repl_running = true;

bool os_get_is_running()
{
    return is_running;
}

void os_set_is_running(bool isRunning)
{
    is_running = isRunning;
}