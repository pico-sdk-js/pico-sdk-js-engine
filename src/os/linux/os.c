#include "os.h"

#include <stdio.h>
#include <fcntl.h> 

int init_fd;
int stdin_fd;

void os_init()
{
    stdin_fd = stdin->_fileno;
    init_fd = fcntl(stdin_fd, F_GETFD);
    fcntl(stdin_fd, F_SETFL, O_NONBLOCK);
}

void os_cleanup()
{
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

