#include "timer.h"

static bool IRAM_ATTR timer_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);

/* Function to init and config timer for current time */
void timer_init(void)
{
    gptimer_handle_t gptimer_handle;
    gptimer_event_callbacks_t gptimer_event_callbacks = 
    {
        .on_alarm = timer_handler,
    };

    gptimer_config_t gptimer_config = 
    {
        .intr_priority = 0,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz
    };

    gptimer_alarm_config_t gptimer_alarm_config = 
    {
        .alarm_count = 1 * 1000 * 1000,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };

    /* Strict ordering for function call */

    // 1. Create a timer instance
    ESP_ERROR_CHECK(gptimer_new_timer(&gptimer_config, &gptimer_handle));

    // 2. Register callback
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_handle, &gptimer_event_callbacks, NULL));

    // 3. Enable the timer
    ESP_ERROR_CHECK(gptimer_enable(gptimer_handle));

    // 4. Set alarm config
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer_handle, &gptimer_alarm_config));

    // 5. Start the timer
    ESP_ERROR_CHECK(gptimer_start(gptimer_handle));
}

/* timer handler */
static bool IRAM_ATTR timer_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    /* increase time in 1 sec*/
    portENTER_CRITICAL_ISR(&m_TIME);
    now++;
    portEXIT_CRITICAL_ISR(&m_TIME);

    /* set false */
    return false; 
}