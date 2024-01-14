#include "repl.h"

#include <string.h>
#include "jerryscript.h"
#include "os.h"
#include "io.h"

#define CR 13

typedef void (*CommandCallback)();

char strg[100];
int lp = 0;

void psj_repl_init()
{

}

void psj_repl_cycle()
{
    char chr = os_getchar_timeout_us(0);
    while (os_getchar_timeout_us_is_valid(chr))
    {
        strg[lp] = chr;
        if (chr == CR || lp == (sizeof(strg) - 1))
        {
            strg[lp] = 0; // terminate string

            jerry_value_t parse_val = jerry_parse(NULL, 0, (const jerry_char_t *)strg, lp, JERRY_PARSE_STRICT_MODE);

            if (jerry_value_is_error(parse_val))
            {
                psj_print_unhandled_exception(parse_val);
            }
            else
            {
                jerry_value_t ret_val = jerry_run(parse_val);
                psj_print_value(ret_val);
                jerry_release_value(ret_val);
            }

            jerry_release_value(parse_val);
            lp = 0; // reset string buffer pointer

            break;
        }
        else
        {
            lp++;
        }

        chr = os_getchar_timeout_us(0);
    }
}

void psj_repl_cleanup()
{

}