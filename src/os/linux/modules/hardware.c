// THIS FILE IS AUTOGENERATED
// Do not directly modify this file. Instead, consider adding to 
// /script/native-modules.json or the /script/*.handlebars templates.
// 
// To regenerate these files run 'node build --gen'
//

#include "modules.h"

#include <sys/types.h>

#include "jerryscript.h"
#include "jerry_helper.h"
#include "callback_engine.h"

static jerry_value_t time_us_32_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 0
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "time_us_32()");
        goto cleanup;
    }


    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "time_us_32();");
    const uint32_t v = 0;
    ret_val = jerry_create_number(v);

cleanup:

    return ret_val;
}

static jerry_value_t time_us_64_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 0
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "time_us_64()");
        goto cleanup;
    }


    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "time_us_64();");
    const uint64_t v = 0;
    ret_val = jerry_create_number(v);

cleanup:

    return ret_val;
}

static jerry_value_t busy_wait_us_32_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 1
        || !jerry_value_is_number(args_p[0])
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "busy_wait_us_32(number)");
        goto cleanup;
    }

    uint32_t delay_us = psj_jerry_to_uint32_t(args_p[0]);

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "busy_wait_us_32(%i);", delay_us);
    
    ret_val = jerry_create_undefined();

cleanup:

    return ret_val;
}

static jerry_value_t busy_wait_us_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 1
        || !jerry_value_is_number(args_p[0])
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "busy_wait_us(number)");
        goto cleanup;
    }

    uint64_t delay_us = psj_jerry_to_uint64_t(args_p[0]);

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "busy_wait_us(%i);", delay_us);
    
    ret_val = jerry_create_undefined();

cleanup:

    return ret_val;
}

static jerry_value_t busy_wait_ms_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 1
        || !jerry_value_is_number(args_p[0])
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "busy_wait_ms(number)");
        goto cleanup;
    }

    uint32_t delay_ms = psj_jerry_to_uint32_t(args_p[0]);

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "busy_wait_ms(%i);", delay_ms);
    
    ret_val = jerry_create_undefined();

cleanup:

    return ret_val;
}

static jerry_value_t busy_wait_until_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 1
        || !jerry_value_is_number(args_p[0])
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "busy_wait_until(number)");
        goto cleanup;
    }

    uint64_t t = psj_jerry_to_uint64_t(args_p[0]);

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "busy_wait_until(%i);", t);
    
    ret_val = jerry_create_undefined();

cleanup:

    return ret_val;
}

static jerry_value_t time_reached_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 1
        || !jerry_value_is_number(args_p[0])
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "time_reached(number)");
        goto cleanup;
    }

    uint64_t t = psj_jerry_to_uint64_t(args_p[0]);

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "time_reached(%i);", t);
    const bool v = true;
    ret_val = jerry_create_boolean(v);

cleanup:

    return ret_val;
}

static jerry_value_t hardware_alarm_claim_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 1
        || !jerry_value_is_number(args_p[0])
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "hardware_alarm_claim(number)");
        goto cleanup;
    }

    uint32_t alarm_num = psj_jerry_to_uint32_t(args_p[0]);

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "hardware_alarm_claim(%i);", alarm_num);
    
    ret_val = jerry_create_undefined();

cleanup:

    return ret_val;
}

static jerry_value_t hardware_alarm_claim_unused_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 1
        || !jerry_value_is_boolean(args_p[0])
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "hardware_alarm_claim_unused(boolean)");
        goto cleanup;
    }

    bool required = psj_jerry_to_bool(args_p[0]);

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "hardware_alarm_claim_unused(%i);", required);
    const int32_t v = 0;
    ret_val = jerry_create_number(v);

cleanup:

    return ret_val;
}

static jerry_value_t hardware_alarm_unclaim_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 1
        || !jerry_value_is_number(args_p[0])
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "hardware_alarm_unclaim(number)");
        goto cleanup;
    }

    uint32_t alarm_num = psj_jerry_to_uint32_t(args_p[0]);

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "hardware_alarm_unclaim(%i);", alarm_num);
    
    ret_val = jerry_create_undefined();

cleanup:

    return ret_val;
}

static jerry_value_t hardware_alarm_is_claimed_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 1
        || !jerry_value_is_number(args_p[0])
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "hardware_alarm_is_claimed(number)");
        goto cleanup;
    }

    uint32_t alarm_num = psj_jerry_to_uint32_t(args_p[0]);

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "hardware_alarm_is_claimed(%i);", alarm_num);
    const bool v = true;
    ret_val = jerry_create_boolean(v);

cleanup:

    return ret_val;
}

static jerry_value_t hardware_alarm_set_callback_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    jerry_value_t ret_val;

    if (
        args_count != 2
        || !jerry_value_is_number(args_p[0])
        || !jerry_value_is_function(args_p[1]
        || !jerry_value_is_null(args_p[1]))
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "hardware_alarm_set_callback(number, function)");
        goto cleanup;
    }

    uint32_t callback_id = psj_jerry_to_uint32_t(args_p[0]);
    jerry_value_t callback = args_p[1];

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "hardware_alarm_set_callback(%d, [Function])", callback_id);
    if (jerry_value_is_null(callback))
    {
        remove_callback(CALLBACK_HARDWARE_ALARM, callback_id);
    }
    else
    {
        register_callback(CALLBACK_HARDWARE_ALARM, callback_id, callback);
    }

    ret_val = jerry_create_undefined();

cleanup:

    return ret_val;
}

jerry_value_t get_hardware_module()
{
    jerry_value_t module = jerry_create_object();
    jerry_value_t handler;

    handler = jerry_create_external_function(time_us_32_handler);
    psj_jerry_set_property(module, "time_us_32", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(time_us_64_handler);
    psj_jerry_set_property(module, "time_us_64", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(busy_wait_us_32_handler);
    psj_jerry_set_property(module, "busy_wait_us_32", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(busy_wait_us_handler);
    psj_jerry_set_property(module, "busy_wait_us", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(busy_wait_ms_handler);
    psj_jerry_set_property(module, "busy_wait_ms", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(busy_wait_until_handler);
    psj_jerry_set_property(module, "busy_wait_until", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(time_reached_handler);
    psj_jerry_set_property(module, "time_reached", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(hardware_alarm_claim_handler);
    psj_jerry_set_property(module, "hardware_alarm_claim", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(hardware_alarm_claim_unused_handler);
    psj_jerry_set_property(module, "hardware_alarm_claim_unused", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(hardware_alarm_unclaim_handler);
    psj_jerry_set_property(module, "hardware_alarm_unclaim", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(hardware_alarm_is_claimed_handler);
    psj_jerry_set_property(module, "hardware_alarm_is_claimed", handler);
    jerry_release_value(handler);

    handler = jerry_create_external_function(hardware_alarm_set_callback_handler);
    psj_jerry_set_property(module, "hardware_alarm_set_callback", handler);
    jerry_release_value(handler);

    return module;
}