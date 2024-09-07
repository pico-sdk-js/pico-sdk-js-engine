#include "psj.h"

#include <string.h>

#include "jerryscript.h"
#include "jerryscript-port.h"

#include "uthash.h"

#define CR '\r'
#define NL '\n'
#define CTRLZ 0x1A
#define BS 0x7F

// JSON inputs can be up to 1.5kb
#define MAX_INPUT_LENGTH (1024+512)

char strg[MAX_INPUT_LENGTH];
int lp = 0;

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

jerry_value_t psj_call_command(jerry_value_t cmd_object)
{
    struct command *cmd = NULL;
    jerry_char_t *name = psj_jerry_get_string_property(cmd_object, "cmd");
    uint32_t etag = psj_jerry_get_uint32_property(cmd_object, "etag");
    jerry_value_t response = jerry_create_object();
    jerry_value_t error_obj = 0;

    if (etag != 0)
    {
        psj_jerry_set_uint32_property(response, "etag", etag);
    }

    if (name != NULL)
    {
        psj_jerry_set_string_property(response, "cmd", name);
    }
    else
    {
        error_obj = psj_jerry_create_error_obj(MISSING_ARG, "cmd");
        goto has_error;
    }

    HASH_FIND_STR(_commands, name, cmd);

    if (cmd == NULL)
    {
        error_obj = psj_jerry_create_error_obj(UNKNOWN_CMD, name);
        goto has_error;
    }

    jerry_value_t args = psj_jerry_get_property(cmd_object, "args");
    jerry_value_t response_value = (*(cmd->command))(args);

    psj_jerry_set_property(response, "value", response_value);
    jerry_release_value(response_value);

    jerry_release_value(args);

has_error:
    if (error_obj != 0)
    {
        psj_jerry_set_property(response, "value", error_obj);
        jerry_release_value(error_obj);
    }

cleanup:

    if (name != NULL)
    {
        free(name);
    }

    return response;
}

void psj_repl_init()
{
    // ## FLASH COMMANDS

    // List files in file system
    psj_add_command("ls", psj_ls_command);

    // Write to file system
    psj_add_command("write", psj_write_command);

    // Read from file system
    psj_add_command("read", psj_read_command);

    // Delete file from file system
    psj_add_command("delete", psj_delete_command);

    // Reformat file system
    psj_add_command("format", psj_format_command);
    
    // ## SYSTEM COMMANDS

    // View stats about the system
    psj_add_command("stats", psj_stats_command);
    
    // Run a command, then resume execution
    psj_add_command("exec", psj_exec_command);

    // Reboot the entire Pico device
    psj_add_command("restart", psj_restart_command);

    // Exit Everything (only available on linux)
    psj_add_command("quit", psj_quit_command);

    // ## EXECUTION COMMANDS

    // Stop & Load the passed in program stored locally in flash
    psj_add_command("run", psj_run_command);

    // Stop & Load the NOOP program
    psj_add_command("kill", psj_kill_command);
}

void psj_repl_call_command_safe(void *cmdPtr)
{
    jerry_value_t cmd = *(jerry_value_t*)cmdPtr;
    jerry_value_t response = psj_call_command(cmd);
    jerry_char_t *jsonValue = psj_jerry_stringify(response);

    printf("%s\n", jsonValue);
    fflush(stdout);

    free(jsonValue);
    jerry_release_value(response);
}

void psj_repl_cycle()
{
    char chr = os_getchar_timeout_us(0);
    while (os_getchar_timeout_us_is_valid(chr))
    {
        os_process_input(chr, strg, MAX_INPUT_LENGTH, &lp);

        if (chr == CR || lp == (sizeof(strg) - 1))
        {
            strg[lp] = 0; // terminate string

            jerry_value_t cmd = jerry_json_parse(strg, lp);
            if (jerry_value_is_error(cmd))
            {
                psj_print_unhandled_exception(cmd);
            }
            else
            {
                // multi-core not currently supported. When support is 
                // added, then will need to consider interrupt suspension 
                // when calling a user command
                // 
                // push_interrupt_suspension();

                psj_repl_call_command_safe(&cmd);

                // pop_interrupt_suspension();
            }

            jerry_release_value(cmd);

            lp = 0; // reset string buffer pointer

            break;
        }

        chr = os_getchar_timeout_us(0);
    }
}

void psj_repl_cleanup()
{
    struct command *currentCmd, *tmp;

    HASH_ITER(hh, _commands, currentCmd, tmp) {
        HASH_DEL(_commands, currentCmd);
        free(currentCmd);
    }

    HASH_CLEAR(hh, _commands);
}