#include <assert.h>
#include <stdlib.h>

#include "os.h"
#include "interrupt_state.h"
#include "jerryscript-port-psj.h"

uint32_t state;
uint32_t stack_count = 0;

bool is_interrupts_suspended()
{
    return stack_count != 0;
}

void push_interrupt_suspension()
{
    assert(stack_count == 0);

    state = os_save_and_disable_interrupts();
    stack_count++;
}

void pop_interrupt_suspension()
{
    stack_count--;
    assert(stack_count == 0);

    os_restore_interrupts(state);

    psj_jerry_port_log_flush();
}