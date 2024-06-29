#include "core1.h"
#include "repl.h"

#include <assert.h>
#include <pthread.h>
#include <jerryscript-port.h>

pthread_t tid = 0;
pthread_attr_t attr;

volatile bool _stopTriggered = false;
volatile bool _threadRunning = false;

static jerry_value_t vm_exec_stop_callback (void *user_p)
{
    if (_stopTriggered) {
        return jerry_create_string("Script aborted");
    }

    return jerry_create_undefined();
}

static void *thread_runner(void *)
{
    // verify not run on core 1 thread
    assert(tid != 0);
    assert(pthread_self() == tid);

    _threadRunning = true;

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "thread_runner running on thread %i", tid);
    psj_repl_run_flash(vm_exec_stop_callback);

    _threadRunning = false;
}

uint get_core_number()
{
    if (pthread_self() == tid)
        return 1;

    return 0;
}

void init_core1()
{
    // core 1 thread not running
    assert(tid == 0);

    pthread_attr_init(&attr);
}

void start_core1()
{
    // core 1 thread not running
    assert(tid == 0);

    _stopTriggered = false;
    pthread_create(&tid, &attr, thread_runner, NULL);
}

void kill_core1()
{
    if (tid != 0)
    {
        _stopTriggered = true;
        
        pthread_join(tid, NULL);
        tid = 0;
    }
}

void restart_core1()
{
    // verify not run on core 1 thread
    assert(pthread_self() != tid);
}

void cleanup_core1()
{
    kill_core1();
}
