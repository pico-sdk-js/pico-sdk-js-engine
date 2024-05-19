#include <malloc.h>

#include "psj.h"
#include "jerryscript.h"

uint32_t getTotalHeap(void)
{
    extern char __StackLimit, __bss_end__;

    return &__StackLimit - &__bss_end__;
}

jerry_value_t psj_stats_command(jerry_value_t request_args)
{
    jerry_value_t value = jerry_create_object();
    psj_jerry_set_string_property(value, "name", TARGET_NAME);
    psj_jerry_set_string_property(value, "version", TARGET_VERSION);

    uint32_t totalRam = getTotalHeap();
    psj_jerry_set_uint32_property(value, "total ram", totalRam);
    struct mallinfo m = mallinfo();

    uint32_t usedRam = m.uordblks;
    psj_jerry_set_uint32_property(value, "used ram", usedRam);
    psj_jerry_set_uint32_property(value, "available ram", totalRam - usedRam);

    jerry_heap_stats_t out_stats;
    if (jerry_get_memory_stats(&out_stats))
    {
        psj_jerry_set_string_property(value, "js engine", "JerryScript");
        psj_jerry_set_uint32_property(value, "js version", out_stats.version);
        psj_jerry_set_uint32_property(value, "js bytes", out_stats.allocated_bytes);
        psj_jerry_set_uint32_property(value, "js peak bytes", out_stats.peak_allocated_bytes);
        psj_jerry_set_uint32_property(value, "js size", out_stats.size);
    }

    return value;
}
