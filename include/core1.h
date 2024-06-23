#ifndef __CORE1_H
#define __CORE1_H

#include <sys/types.h>
#include <stdbool.h>

uint get_core_number();

bool get_is_core_running();

void init_core1();

void start_core1();

void restart_core1();

void kill_core1();

void cleanup_core1();

#endif // __CORE1_H
