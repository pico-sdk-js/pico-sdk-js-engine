#include "modules.h"

#include "jerryscript.h"
#include "jerry_helper.h"

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

    return module;
}