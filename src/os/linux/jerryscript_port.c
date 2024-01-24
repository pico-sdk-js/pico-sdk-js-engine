#include "jerryscript-port.h"

#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

double jerry_port_get_local_time_zone_adjustment(double unix_ms, bool is_utc)
{
    struct tm tm;
    time_t now = (time_t)(unix_ms / 1000);
    localtime_r(&now, &tm);
    if (!is_utc)
    {
        now -= tm.tm_gmtoff;
        localtime_r(&now, &tm);
    }
    return ((double)tm.tm_gmtoff) * 1000;
}

double jerry_port_get_current_time(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
    {
        return 0;
    }

    return ((double)tv.tv_sec) * 1000.0 + ((double)tv.tv_usec) / 1000.0;
}

size_t jerry_port_normalize_path(const char *in_path_p, char *out_buf_p, size_t out_buf_size, char *base_file_p)
{
    size_t ret = 0;

    char *base_dir_p = dirname(base_file_p);
    const size_t base_dir_len = strnlen(base_dir_p, PATH_MAX);
    const size_t in_path_len = strnlen(in_path_p, PATH_MAX);
    char *path_p = (char *)malloc(base_dir_len + 1 + in_path_len + 1);

    memcpy(path_p, base_dir_p, base_dir_len);
    memcpy(path_p + base_dir_len, "/", 1);
    memcpy(path_p + base_dir_len + 1, in_path_p, in_path_len + 1);

    char *norm_p = realpath(path_p, NULL);
    free(path_p);

    if (norm_p != NULL)
    {
        const size_t norm_len = strnlen(norm_p, out_buf_size);
        if (norm_len < out_buf_size)
        {
            memcpy(out_buf_p, norm_p, norm_len + 1);
            ret = norm_len;
        }

        free(norm_p);
    }

    return ret;
}
