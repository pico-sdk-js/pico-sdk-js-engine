#include "os.h"

#include <sys/ioctl.h>
#include <termios.h>


int char_pending() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

void os_init()
{
}

void os_cleanup()
{
}

void os_wait_for_ready()
{
}

int os_getchar_timeout_us(uint32_t timeout)
{
    if (char_pending())
    {
        int chr = getchar();
        if (chr == '\n')
        {
            return '\r';
        }

        return chr;
    }

    return -1;
}

bool os_getchar_timeout_us_is_valid(int chr)
{
    return chr != -1;
}

