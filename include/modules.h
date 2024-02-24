#ifndef __MODULES_H
#define __MODULES_H

#include "jerryscript.h"

void init_modules();

jerry_value_t get_module(jerry_char_t *name);

void reset_modules();

jerry_value_t get_hardware_module();

#endif // __MODULES_H