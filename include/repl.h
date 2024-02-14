#ifndef __REPL_H
#define __REPL_H

#include "jerryscript.h"

typedef jerry_value_t (*CommandCallback)(jerry_value_t request_args);

void psj_add_command(char *name, CommandCallback cb);

void psj_repl_init();

void psj_repl_cycle();

void psj_repl_cleanup();

void psj_repl_run_flash();

jerry_value_t psj_exec_command(jerry_value_t request_args);
jerry_value_t psj_stats_command(jerry_value_t request_args);
jerry_value_t psj_quit_command(jerry_value_t request_args);

#endif // __REPL_H