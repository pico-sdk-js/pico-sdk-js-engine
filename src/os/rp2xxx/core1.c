#include "core1.h"
#include "repl.h"

#include "pico/multicore.h"
#include "hardware/gpio.h"

#include <assert.h>
#include <jerryscript-port.h>

volatile bool _stopTriggered = false;
volatile bool _threadRunning = false;

static jerry_value_t vm_exec_stop_callback (void *user_p)
{
    if (_stopTriggered) {
        return jerry_create_string("Script aborted");
    }

    return jerry_create_undefined();
}

static void thread_runner()
{
    _threadRunning = true;

    // Allow lockout of core1
    multicore_lockout_victim_init();
    
    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "thread_runner running on core 1");
    
    psj_repl_run_flash(vm_exec_stop_callback);

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "thread_runner exiting core 1");

    _threadRunning = false;
}

uint get_core_number()
{
    return get_core_num();
}

bool get_is_core_running()
{
    return _threadRunning;
}

void init_core1()
{
    assert(!_threadRunning);

    multicore_reset_core1();
}

void start_core1()
{
    assert(!_threadRunning);

    _stopTriggered = false;
    multicore_launch_core1(thread_runner);
}

void kill_core1()
{
    if (_threadRunning)
    {
        _stopTriggered = true;
        
        do
        {
            sleep_ms(100);
        } while (_threadRunning);
    }
}

void restart_core1()
{
    kill_core1();
    start_core1();
}

void cleanup_core1()
{
    kill_core1();
}
