#include "jerryscript-port.h"

#include <string.h>
#include "pico/stdlib.h"

double jerry_port_get_local_time_zone_adjustment(double unix_ms, bool is_utc)
{
    return 0;
}

double jerry_port_get_current_time(void)
{
    uint64_t t = time_us_64();
    return (double)(t / 1000u);
}

size_t jerry_port_normalize_path(const char *in_path_p, char *out_buf_p, size_t out_buf_size, char *base_file_p)
{
    size_t ret = 0;

    (void)base_file_p; /* unused */

    /* Do nothing, just copy the input. */
    const size_t in_path_len = strnlen(in_path_p, out_buf_size);
    if (in_path_len < out_buf_size)
    {
        memcpy(out_buf_p, in_path_p, in_path_len + 1);
        ret = in_path_len;
    }

    return ret;
}
