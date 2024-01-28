#include "os.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>

#define CR '\r'
#define NL '\n'
#define CTRLZ 0x1A
#define BS 0x7F

int init_fd;
int stdin_fd;
struct termios tcOrig;

bool is_running = true;

// reserve 1MB of flash buffer
static char flashFile[] = "flash.bin";
static uint8_t *flash_buffer = NULL;

void os_init()
{
    // Initialize terminal
    stdin_fd = stdin->_fileno;
    init_fd = fcntl(stdin_fd, F_GETFD);
    fcntl(stdin_fd, F_SETFL, O_NONBLOCK);

    tcgetattr(stdin_fd, &tcOrig);
    struct termios tcNew;
    memcpy(&tcNew, &tcOrig, sizeof(tcNew));

    tcNew.c_lflag &= ~(ICANON | ECHO);
    tcNew.c_cc[VSUSP] = 0;

    tcsetattr(stdin_fd, TCSANOW, &tcNew);

    // Initialize "flash" memory
    flash_buffer = malloc(FLASH_TARGET_SIZE);
    FILE *fp = fopen(flashFile, "r");
    if (fp != NULL)
    {
        fread(flash_buffer, FLASH_TARGET_SIZE, 1, fp);
        fclose(fp);
    }
}

void os_cleanup()
{
    // Cleanup "flash" memory
    FILE *fp = fopen(flashFile, "w");
    if (fp != NULL)
    {
        fwrite(flash_buffer, FLASH_TARGET_SIZE, 1, fp);
        fclose(fp);
    }

    free(flash_buffer);
    flash_buffer = NULL;

    // Restore terminal
    tcsetattr(stdin_fd, TCSANOW, &tcOrig);
    fcntl(stdin_fd, F_SETFL, init_fd);
}

void os_wait_for_ready()
{
}

void os_reset_usb_boot(uint32_t usb_activity_gpio_pin_mask, uint32_t disable_interface_mask)
{
}

int os_getchar_timeout_us(uint32_t timeout)
{
    int chr = getchar();
    if (chr == '\n')
    {
        return '\r';
    }

    return chr;
}

bool os_getchar_timeout_us_is_valid(int chr)
{
    return chr != -1;
}

bool os_get_is_running()
{
    return is_running;
}

void os_exit()
{
    is_running = false;
}

void os_process_input(char c, char *s, int max_length, int *sp)
{
    switch (c)
    {
    case BS:
        if (*sp > 0)
        {
            printf("\b \b");
            *sp -= 1;
        }
        break;
    case NL:
    case CR:
        printf("\n");
        break;
    default:
        if (isprint(c) && *sp < max_length - 1)
        {
            putc(c, stdout);
            s[(*sp)++] = c;
        }
        break;
    }
}

void os_flash_range_erase(uint32_t flash_offs, size_t count)
{
    uint32_t offset = (flash_offs-FLASH_TARGET_OFFSET);
    assert(offset >= 0);

    for (size_t i = 0; i < count; i++)
    {
        flash_buffer[offset + i] = 0;
    }    
}

void os_flash_range_program(uint32_t flash_offs, const uint8_t *data, size_t count)
{
    uint32_t offset = (flash_offs-FLASH_TARGET_OFFSET);
    assert(offset >= 0);

    for (size_t i = 0; i < count; i++)
    {
        flash_buffer[offset + i] = data[i];
    }   
}

uint8_t *os_get_flash_buffer()
{
    return (uint8_t*)flash_buffer;
}

uint32_t os_save_and_disable_interrupts()
{
    return 1;
}

void os_restore_interrupts(uint32_t status)
{
    assert(status == 1);
}
