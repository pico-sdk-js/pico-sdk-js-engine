#ifndef __REPL_H
#define __REPL_H

#include "jerryscript.h"

typedef jerry_value_t (*CommandCallback)(jerry_value_t request_args);

void psj_add_command(char *name, CommandCallback cb);

void psj_repl_init();

void psj_repl_cycle();

void psj_repl_cleanup();

jerry_value_t psj_read_command(jerry_value_t request_args);
jerry_value_t psj_write_command(jerry_value_t request_args);
jerry_value_t psj_ls_command(jerry_value_t request_args);
jerry_value_t psj_delete_command(jerry_value_t request_args);
jerry_value_t psj_format_command(jerry_value_t request_args);

jerry_value_t psj_exec_command(jerry_value_t request_args);
jerry_value_t psj_stats_command(jerry_value_t request_args);

jerry_value_t psj_run_command(jerry_value_t request_args);
jerry_value_t psj_kill_command(jerry_value_t request_args);
jerry_value_t psj_restart_command(jerry_value_t request_args);

jerry_value_t psj_quit_command(jerry_value_t request_args);

#endif // __REPL_H