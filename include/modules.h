#ifndef __MODULES_H
#define __MODULES_H

#include "jerryscript.h"

void init_modules();

jerry_value_t get_module(jerry_char_t *name);

void reset_modules();

jerry_value_t get_hardware_module();

// External modules
jerry_value_t gpio_set_irq_enabled_with_callback_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count);

#endif // __MODULES_H