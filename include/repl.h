#ifndef __REPL_H
#define __REPL_H

typedef void (*CommandCallback)();

void psj_add_command(char *name, CommandCallback cb);

void psj_repl_init();

void psj_repl_cycle();

void psj_repl_cleanup();

void psj_repl_run_flash();

#endif // __REPL_H