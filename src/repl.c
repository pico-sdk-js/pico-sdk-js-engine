#include "repl.h"

#include <string.h>

#include "jerryscript.h"
#include "jerryscript-port.h"

#include "uthash.h"

#include "os.h"
#include "io.h"
#include "flash.h"

#define CR '\r'
#define NL '\n'
#define CTRLZ 0x1A
#define BS 0x7F

typedef void (*CommandCallback)();

#define MAX_INPUT_LENGTH 100
#define MAX_SCRIPT_LENGTH 1024

char strg[MAX_INPUT_LENGTH];
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
    os_exit();
}

void psj_flash_command()
{
    printf("WRITE DATA (^Z to end)\n");

    jerry_char_t script[MAX_SCRIPT_LENGTH];
    int bp = 0;

    bool is_done = false;
    while (!is_done)
    {
        char chr = os_getchar_timeout_us(0);
        if (os_getchar_timeout_us_is_valid(chr))
        {
            switch (chr)
            {
                case CTRLZ:
                    script[bp] = 0;
                    psj_flash_save(script);
                    is_done = true;
                    printf("\nSAVED\n");
                    break;
                default:
                    os_process_input(chr, script, MAX_SCRIPT_LENGTH, &bp);
                    break;
            }
        }
    }
}

void psj_restart_command()
{
    psj_repl_run_flash();
}

void psj_dump_flash_command()
{
    jerry_char_t script[MAX_SCRIPT_LENGTH];
    int scriptLen = psj_flash_read(script, MAX_SCRIPT_LENGTH);
    if (scriptLen != 0)
    {
        printf("%s\n", script);
    }
}

void psj_stats_command()
{
    jerry_heap_stats_t stats;
    if (!jerry_get_memory_stats (&stats))
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "FAILED TO GET MEMORY STATS\n");
        return;
    }

    printf("version: %lu\nsize: %lu\nallocated bytes: %lu\npeak allocated bytes: %lu\n\n", stats.version, stats.size, stats.allocated_bytes, stats.peak_allocated_bytes);
}

void psj_gc_command()
{
    jerry_gc(JERRY_GC_PRESSURE_LOW);
}

void psj_bootsel_command()
{
    os_reset_usb_boot(0, 0);
}

void psj_repl_init()
{
    psj_add_command(".flash", psj_flash_command);
    psj_add_command(".quit", psj_quit_command);
    psj_add_command(".dump", psj_dump_flash_command);
    psj_add_command(".restart", psj_restart_command);    
    psj_add_command(".stats", psj_stats_command);
    psj_add_command(".gc", psj_gc_command);
    psj_add_command(".bootsel", psj_bootsel_command);
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
        os_process_input(chr, strg, MAX_INPUT_LENGTH, &lp);

        if (chr == CR || lp == (sizeof(strg) - 1))
        {
            strg[lp] = 0; // terminate string

            if (strg[0] == '.')
            {
                if (!psj_call_command(strg))
                {
                    jerry_port_log(JERRY_LOG_LEVEL_ERROR, "ERROR: Command '%s' not found\n", strg);
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

        chr = os_getchar_timeout_us(0);
    }
}

void psj_repl_cleanup()
{
    HASH_CLEAR(hh, _commands);
}

void psj_repl_run_flash()
{
    jerry_char_t script[MAX_SCRIPT_LENGTH];
    int scriptLen = psj_flash_read(script, MAX_SCRIPT_LENGTH);
    if (scriptLen == 0)
    {
        return;
    }
    
    jerry_value_t parsed_code = jerry_parse("main.js", strlen("main.js"), script, strlen(script), JERRY_PARSE_STRICT_MODE | JERRY_PARSE_MODULE);

    if (jerry_value_is_error(parsed_code))
    {
        psj_print_unhandled_exception(parsed_code);
    }
    else
    {
        jerry_value_t ret_val = jerry_run(parsed_code);
        if (jerry_value_is_error(ret_val))
        {
            psj_print_unhandled_exception(ret_val);
        }
        jerry_release_value(ret_val);
    }

    /* Parsed source code must be freed */
    jerry_release_value(parsed_code);
}