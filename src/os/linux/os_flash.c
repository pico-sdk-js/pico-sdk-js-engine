#include "os.h"

#include <stdlib.h>
#include <string.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// reserve 1MB of flash buffer
static char flashFile[] = "flash.js";
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
    free(flash_buffer);
    flash_buffer = NULL;
}

void os_flash_save(char *jsFile)
{
    // Write to both flash buffer...
    strncpy(flash_buffer, jsFile, flash_buffer_size);

    // ...and to flash file
    FILE *fp = fopen(flashFile, "w");
    if (fp != NULL)
    {
        fwrite(jsFile, MIN(strlen(jsFile), flash_buffer_size), 1, fp);
        fclose(fp);
    }
}

char* os_flash_read()
{
    return (char*)flash_buffer;
}

