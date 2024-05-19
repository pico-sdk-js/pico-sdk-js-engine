#include "modules.h"

#include <string.h>
#include <assert.h>

#include "uthash.h"
#include "jerry_helper.h"
#include "jerryscript-ext/handler.h"

typedef jerry_value_t (*ModuleCreateCallback)();

struct module
{
    char *key; /* key */
    ModuleCreateCallback create;
    jerry_value_t moduleValue;
    UT_hash_handle hh; /* makes this structure hashable */
};

struct module *__modules = NULL;

void add_module(const jerry_char_t *name, ModuleCreateCallback callback)
{
    jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "add_module: adding module '%s'", name);

    struct module *module;
    module = malloc(sizeof(*module));
    module->key = S(name);
    module->create = callback;
    module->moduleValue = 0;

    HASH_ADD_STR(__modules, key, module);
}

void init_modules()
{
    // Setup module callbacks
    assert(__modules == NULL);

    add_module("pico/hardware", get_hardware_module);

    // Setup global

    /* Register 'print' function from the extensions */
    jerryx_handler_register_global((const jerry_char_t *)"print", jerryx_handler_print);

}

jerry_value_t get_module(jerry_char_t *name)
{
    struct module *m;
    jerry_value_t module_value;

    jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "get_module: searching for module '%s'", name);
    HASH_FIND_STR(__modules, name, m);
    if (m == NULL)
    {
        jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "get_module: module not found");
        module_value = jerry_create_undefined();
    }
    else if (m->moduleValue != 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "get_module: module found, reusing");
        module_value = m->moduleValue;
    }
    else
    {
        jerry_port_log(JERRY_LOG_LEVEL_DEBUG, "get_module: module found, creating");
        module_value = m->moduleValue = m->create();
    }

    return module_value;
}

void reset_modules()
{
    if (__modules != NULL)
    {
        struct module *currentModule, *tmp;

        HASH_ITER(hh, __modules, currentModule, tmp) {
            HASH_DEL(__modules, currentModule);
            if (currentModule->moduleValue != 0)
            {
                jerry_release_value(currentModule->moduleValue);
            }
            free(currentModule->key);
            free(currentModule);
        }
    }    
}
