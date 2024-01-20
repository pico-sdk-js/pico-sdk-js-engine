#include "os.h"

#include <stdlib.h>
#include <string.h>

// reserve 1MB of flash buffer
static char flashFile[] = "flash.bin";
static uint8_t *flash_buffer = NULL;
const uint32_t flash_buffer_size = 1024*1024;

uint32_t os_get_flash_buffer_size()
{
    return flash_buffer_size;
}

void os_flash_init()
{
    flash_buffer = malloc(flash_buffer_size);
    FILE *fp = fopen(flashFile, "r");
    if (fp != NULL)
    {
        fread(flash_buffer, flash_buffer_size, 1, fp);
        fclose(fp);
    }
}

void os_flash_cleanup()
{
    FILE *fp = fopen(flashFile, "w");
    if (fp != NULL)
    {
        fwrite(flash_buffer, flash_buffer_size, 1, fp);
        fclose(fp);
    }


    free(flash_buffer);
    flash_buffer = NULL;
}

void os_flash_save(char *jsFile)
{
    strncpy(flash_buffer, jsFile, flash_buffer_size);
}

char* os_flash_read()
{
    return (char*)flash_buffer;
}

