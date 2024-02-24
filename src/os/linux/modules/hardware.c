#include "modules.h"

#include <stdlib.h>
#include <sys/time.h>

#include "jerry_helper.h"

int get_seed()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0)
    {
        return 0;
    }

    return tv.tv_usec;
}

jerry_value_t get_hardware_module()
{
    jerry_value_t module = jerry_create_object();
    srand(get_seed());
    psj_jerry_set_uint32_property(module, "foo", rand());
    return module;
}