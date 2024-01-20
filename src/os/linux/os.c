#include "os.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>
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

void signal_received(int v)
{
    printf("Signal Received: %i", v);
}

void os_init()
{
    stdin_fd = stdin->_fileno;
    init_fd = fcntl(stdin_fd, F_GETFD);
    fcntl(stdin_fd, F_SETFL, O_NONBLOCK);

    tcgetattr(stdin_fd, &tcOrig);
    struct termios tcNew;
    memcpy(&tcNew, &tcOrig, sizeof(tcNew));

    tcNew.c_lflag &= ~(ICANON | ECHO);
    tcNew.c_cc[VSUSP] = 0;

    tcsetattr(stdin_fd, TCSANOW, &tcNew);

    os_flash_init();
}

void os_cleanup()
{
    os_flash_cleanup();

    tcsetattr(stdin_fd, TCSANOW, &tcOrig);
    fcntl(stdin_fd, F_SETFL, init_fd);
}

void os_wait_for_ready()
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
