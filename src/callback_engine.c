#include "callback_engine.h"
#include "jerry_helper.h"

#include <assert.h>
#include "uthash.h"

typedef struct __callback_hash {
    uint32_t id;
    jerry_value_t callback;
    UT_hash_handle hh;
} callback_hash;

callback_hash *callbacks = NULL;

uint32_t hash(CALLBACK_TYPE type, uint32_t id)
{
    assert(id <= 0x00FFFFFF);
    
    return type << 24 | id;
}

void register_callback(CALLBACK_TYPE type, uint32_t id, const jerry_value_t value)
{
    assert(jerry_value_is_function(value));

    callback_hash *cb = malloc(sizeof(callback_hash));
    cb->id = hash(type, id);
    cb->callback = value;
    HASH_ADD_INT(callbacks, id, cb);
}

jerry_value_t invoke_callback(CALLBACK_TYPE type, uint32_t id)
{
    callback_hash *cb;
    uint32_t key = hash(type, id);
    HASH_FIND_INT(callbacks, &key, cb);

    if (cb == NULL)
    {
        return psj_jerry_create_error_obj(NO_CALLBACK_REGISTERED, key);
    }

    jerry_value_t this_val = jerry_create_undefined();
    jerry_value_t args[] = { jerry_create_number(id) };
    jerry_value_t result = jerry_call_function(cb->callback, this_val, args, 1);

    jerry_release_value(this_val);
    jerry_release_value(args[0]);
    
    return result;
}

bool has_callback(CALLBACK_TYPE type, uint32_t id)
{
    callback_hash *cb;
    uint32_t key = hash(type, id);
    HASH_FIND_INT(callbacks, &key, cb);

    return cb != NULL;
}

void remove_callback(CALLBACK_TYPE type, uint32_t id)
{
    callback_hash *cb;
    uint32_t key = hash(type, id);
    HASH_FIND_INT(callbacks, &key, cb);

    if (cb != NULL)
    {
        HASH_DEL(callbacks, cb);
        jerry_release_value(cb->callback);        
    }
}

void hardware_alarm_set_callback_wrapper(uint alarm_num)
{
    jerry_value_t result = invoke_callback(CALLBACK_HARDWARE_ALARM, alarm_num);

    if (jerry_value_is_error(result))
    {
        jerry_char_t *error_string = psj_jerry_exception_to_string(result);
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Hardware Alarm Handler error: %s", error_string);
        free(error_string);
    }

    jerry_release_value(result);
}
