#include "modules.h"

#include <sys/types.h>
#include "os.h"

// NOTE: Below handlers do not apply IRQ handlers to the PICO-SDK. Instead they apply them to an internal

static jerry_value_t gpio_add_raw_irq_handler_with_order_priority_masked_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    /*
    void gpio_add_raw_irq_handler_with_order_priority_masked (uint gpio_mask, irq_handler_t handler, uint8_t order_priority)
     Adds a raw GPIO IRQ handler for the specified GPIOs on the current core.
    */
}

static jerry_value_t gpio_add_raw_irq_handler_with_order_priority_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    /*
    static void gpio_add_raw_irq_handler_with_order_priority (uint gpio, irq_handler_t handler, uint8_t order_priority)
     Adds a raw GPIO IRQ handler for a specific GPIO on the current core.
    */
}

static jerry_value_t gpio_add_raw_irq_handler_masked_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    /*
    void gpio_add_raw_irq_handler_masked (uint gpio_mask, irq_handler_t handler)
     Adds a raw GPIO IRQ handler for the specified GPIOs on the current core.
    */
}

static jerry_value_t gpio_add_raw_irq_handler_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    /*
    static void gpio_add_raw_irq_handler (uint gpio, irq_handler_t handler)
     Adds a raw GPIO IRQ handler for a specific GPIO on the current core.
    */
}

static jerry_value_t gpio_remove_raw_irq_handler_masked_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    /*
    void gpio_remove_raw_irq_handler_masked (uint gpio_mask, irq_handler_t handler)
     Removes a raw GPIO IRQ handler for the specified GPIOs on the current core.
    */
}

static jerry_value_t gpio_remove_raw_irq_handler_handler(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args_p[], const jerry_length_t args_count)
{
    /*
    static void gpio_remove_raw_irq_handler (uint gpio, irq_handler_t handler)
     Removes a raw GPIO IRQ handler for the specified GPIO on the current core.
    */
}
