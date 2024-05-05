#include "modules.h"

#include "jerryscript.h"
#include "jerry_helper.h"
#include "callback_engine.h"

#include "hardware/gpio.h"
#include "hardware/irq.h"

#include <sys/types.h>

jerry_value_t gpio_set_irq_enabled_with_callback_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    /*
    void gpio_set_irq_enabled_with_callback (uint gpio, uint32_t event_mask, bool enabled, gpio_irq_callback_t callback)
     Convenience function which performs multiple GPIO IRQ related initializations.
    */

    jerry_value_t ret_val;

    if (
        args_count != 4
        || !jerry_value_is_number(args_p[0])
        || !jerry_value_is_number(args_p[1])
        || !jerry_value_is_boolean(args_p[2])
        || !jerry_value_is_function(args_p[3])
    )
    {
        ret_val = psj_jerry_create_error_obj(RUNTIME_ARG_ERROR, "gpio_set_irq_enabled_with_callback(number, number, boolean, function)");
        goto cleanup;
    }

    uint gpio = psj_jerry_to_uint(args_p[0]);
    uint32_t event_mask = psj_jerry_to_uint32_t(args_p[1]);
    bool enabled = psj_jerry_to_bool(args_p[2]);
    jerry_value_t callback = args_p[3];

    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "gpio_set_irq_enabled_with_callback(%i, %i, %i, [Function]);", gpio, event_mask, enabled);
    
    gpio_set_irq_enabled(gpio, event_mask, enabled);

    if (jerry_value_is_null(callback))
    {
        gpio_set_irq_callback(NULL);
        remove_callback(CALLBACK_GPIO_IRQ, 0);
    }
    else
    {
        register_callback(CALLBACK_GPIO_IRQ, 0, callback);
        gpio_set_irq_callback(gpio_set_irq_callback_wrapper);
    }

    if (enabled) irq_set_enabled(IO_IRQ_BANK0, true);

    ret_val = jerry_create_undefined();

cleanup:

    return ret_val;

}
