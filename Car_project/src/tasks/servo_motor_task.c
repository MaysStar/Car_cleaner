#include "servo_motor_task.h"

static mcpwm_timer_handle_t servo_timer;
static mcpwm_oper_handle_t servo_oper;
static mcpwm_cmpr_handle_t servo_cmpr;
static mcpwm_gen_handle_t servo_gen;

/* Function to set angle in ms for comparator */
void set_angle(int32_t angle)
{
    /* 0.5ms pulse -90, 2ms 90 degree */

    uint32_t duty_us = 500 + ((2000 * angle) / 180);

    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(servo_cmpr, duty_us));
}

/* Servo motor initialization */
void servo_motor_init(void)
{
    mcpwm_timer_config_t mcpwm_timer_config = 
    {
        .group_id = 0,
        .intr_priority = 1,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1 MHz
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 20000, // 50 Hz
        .flags.update_period_on_empty = 1,
    };

    ESP_ERROR_CHECK(mcpwm_new_timer(&mcpwm_timer_config, &servo_timer));
 
    mcpwm_operator_config_t mcpwm_operator_config = 
    {
        .group_id = 0,
        .intr_priority = 1,
        .flags.update_dead_time_on_tez = 1,
    };

    ESP_ERROR_CHECK(mcpwm_new_operator(&mcpwm_operator_config, &servo_oper));

    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(servo_oper, servo_timer));

    mcpwm_comparator_config_t mcpwm_comparator_config = 
    {
        .intr_priority = 1,
        .flags.update_cmp_on_tez = 1,
    };

    ESP_ERROR_CHECK(mcpwm_new_comparator(servo_oper, &mcpwm_comparator_config, &servo_cmpr));

    mcpwm_generator_config_t mcpwm_generator_config = 
    {
        .gen_gpio_num = GPIO_SERVO_MOTOR,
    };

    ESP_ERROR_CHECK(mcpwm_new_generator(servo_oper, &mcpwm_generator_config, &servo_gen));

    // set HIGH 
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(servo_gen, 
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
   
    // set value to compare and action
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(servo_gen,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, servo_cmpr, MCPWM_GEN_ACTION_LOW)));

    ESP_ERROR_CHECK(mcpwm_timer_enable(servo_timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(servo_timer, MCPWM_TIMER_START_NO_STOP));
}

/* Main task for servo motor */
void servo_motor_task(void* pvParameters)
{
    servo_motor_init();

    // set servo in center
    mcpwm_comparator_set_compare_value(servo_cmpr, 1500);
#if SERVO_USE_MQTTS_COMMAND
    static int32_t angle = 0; 
#endif
    //static EventBits_t bits;
    //static bool less_then_20_cm = false;

    while(1)
    {
#if SERVO_USE_MQTTS_COMMAND
        xQueueReceive(q_servo_angle, &angle, portMAX_DELAY);
        set_angle(angle);
        vTaskDelay(pdMS_TO_TICKS(100));        
#endif
#if SERVO_USE_ORDINARY_MODE
            vTaskDelay(pdMS_TO_TICKS(1000)); 
#endif       
    }
}