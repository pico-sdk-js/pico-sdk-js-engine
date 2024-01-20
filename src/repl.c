#include "repl.h"

#include <string.h>

#include "jerryscript.h"
#include "uthash.h"

#include "os.h"
#include "io.h"

#define CR 13

typedef void (*CommandCallback)();

char strg[100];
int lp = 0;
bool start_prompt = true;

struct command
{
    char key[16]; /* key */
    CommandCallback command;
    UT_hash_handle hh; /* makes this structure hashable */
};

struct command *_commands = NULL;

void psj_add_command(char *name, CommandCallback cb)
{
    struct command *cmd;
    cmd = malloc(sizeof(*cmd));

    strcpy(cmd->key, name);
    cmd->command = cb;
    HASH_ADD_STR(_commands, key, cmd);
}

bool psj_call_command(char *name)
{
    struct command *cmd;
    HASH_FIND_STR(_commands, name, cmd);

    if (cmd != NULL)
    {
        (*(cmd->command))();
        return true;
    }

    return false;
}

void psj_quit_command()
{
    printf("QUITTING...\n");
}

void psj_repl_init()
{
    CommandCallback cmd = psj_quit_command;
    psj_add_command(".quit", cmd);
}

void psj_repl_cycle()
{
    if (start_prompt)
    {
        start_prompt = false;
        printf("> ");
    }

    char chr = os_getchar_timeout_us(0);
    while (os_getchar_timeout_us_is_valid(chr))
    {
        strg[lp] = chr;
        if (chr == CR || lp == (sizeof(strg) - 1))
        {
            strg[lp] = 0; // terminate string

            if (strg[0] == '.')
            {
                if (!psj_call_command(strg))
                {
                    jerry_port_log(JERRY_LOG_LEVEL_ERROR, "ERROR: Command not found\n");
                }
            }
            else
            {
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
            }

            lp = 0; // reset string buffer pointer
            start_prompt = true;

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
    HASH_CLEAR(hh, _commands);
}