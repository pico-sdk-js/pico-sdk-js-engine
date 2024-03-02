#ifndef __CALLBACK_ENGINE_H
#define __CALLBACK_ENGINE_H

#include "jerryscript.h"
#include <sys/types.h>

typedef enum __CALLBACK_TYPE {
    CALLBACK_GENERIC = 0,
    CALLBACK_HARDWARE_ALARM = 1
} CALLBACK_TYPE;

uint32_t hash(CALLBACK_TYPE type, uint32_t id);

void register_callback(CALLBACK_TYPE type, uint32_t id, const jerry_value_t value);
jerry_value_t invoke_callback(CALLBACK_TYPE type, uint32_t id);
bool has_callback(CALLBACK_TYPE type, uint32_t id);
void remove_callback(CALLBACK_TYPE type, uint32_t id);

void hardware_alarm_set_callback_wrapper(uint alarm_num);

#endif // __CALLBACK_ENGINE_H
