#include "psj.h"

static const char* __noopFn = "let __noop_running = true; let x = 0; while(__noop_running) { x++; }";
static const char* __stopNoopFn = "__noop_running = false;";
static int8_t __jsengine_state = JSENGINE_STATE_STOPPED;
char* _nextResource = NULL;
char* _currentResource = NULL;

int8_t jsengine_get_state()
{
    return __jsengine_state;
}

void jsengine_set_state(int8_t state)
{
    __jsengine_state = state;
}

void psj_run_javascript(const jerry_char_t *resource_name_p, size_t resource_name_length, const jerry_char_t *source_p, size_t source_size, jerry_vm_exec_stop_callback_t halt_handler)
{
    jerry_value_t parsed_code = jerry_parse(resource_name_p, resource_name_length, source_p, source_size, JERRY_PARSE_STRICT_MODE | JERRY_PARSE_MODULE);

    if (jerry_value_is_error(parsed_code))
    {
        psj_print_unhandled_exception(parsed_code);
    }
    else
    {
        jerry_set_vm_exec_stop_callback(halt_handler, NULL, 1);

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

void psj_run_resource(const jerry_char_t *resource_name_p, jerry_vm_exec_stop_callback_t halt_handler)
{
    if (resource_name_p == NULL)
    {
        psj_run_javascript("", 0, __noopFn, strlen(__noopFn), halt_handler);
    }
    else
    {
        uint32_t scriptLength;
        int err = psj_flash_file_size(resource_name_p, &scriptLength);
        if (err < 0)
        {
            jerry_port_log(JERRY_LOG_LEVEL_TRACE, "psj_flash_file_size('%s') returned %i", resource_name_p, err);
            return;
        }

        jerry_char_t script[scriptLength];
        err = psj_flash_read_all(resource_name_p, script, scriptLength);
        if (err < 0)
        {
            jerry_port_log(JERRY_LOG_LEVEL_TRACE, "psj_flash_read_all('%s') returned %i", resource_name_p, err);
            return;
        }

        psj_run_javascript(resource_name_p, strlen(resource_name_p), script, scriptLength, halt_handler);
    }
}

static jerry_value_t vm_exec_stop_callback (void *user_p)
{
    if (jsengine_get_state() == JSENGINE_STATE_REQ_STOPPING) {
        jsengine_set_state(JSENGINE_STATE_AWAIT_STOPPING);

        if (_currentResource == NULL)
        {
            jerry_value_t ret_val = jerry_eval(__stopNoopFn, strlen(__stopNoopFn), JERRY_PARSE_NO_OPTS);
            jerry_release_value(ret_val);
        }
        else
        {
            return jerry_create_string("Script aborted");
        }
    }
    else
    {
        psj_repl_cycle();
    }

    return jerry_create_undefined();
}

char* psj_get_current_resource()
{
    return _currentResource;
}

void psj_set_next_resource(const char* nextResource)
{
    if (_nextResource != NULL)
    {
        free(_nextResource);
        _nextResource = NULL;
    }

    _nextResource = S(nextResource);

    // force stop & restart
    jsengine_set_state(JSENGINE_STATE_REQ_STOPPING);
}

void psj_run_js_loop()
{
    if (_currentResource != NULL)
    {
        free(_currentResource);
        _currentResource = NULL;
    }

    _currentResource = S(_nextResource);

    // reset next resource so we do a noop loop after _currentResource completes
    if (_nextResource != NULL)
    {
        free(_nextResource);
        _nextResource = NULL;
    }

    jsengine_set_state(JSENGINE_STATE_RUNNING);

    psj_run_resource(_currentResource, vm_exec_stop_callback);

    jsengine_set_state(JSENGINE_STATE_STOPPED);
}