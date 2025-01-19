#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

#include "jerryscript.h"

void init_configuration();
void cleanup_configuration();

jerry_char_t *config_get_autorun();
void config_set_autorun(jerry_char_t *path);

#endif // __CONFIGURATION_H