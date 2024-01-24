#include "os.h"

const uint32_t flash_buffer_size = 1024*1024;

void os_flash_init()
{
}

void os_flash_cleanup()
{
}

void os_flash_save(char *jsFile)
{
}

char* os_flash_read()
{
    return NULL;
}

uint32_t os_get_flash_buffer_size()
{
    return flash_buffer_size;
}
