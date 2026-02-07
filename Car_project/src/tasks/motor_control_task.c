#include "motor_control_task.h"

/* GPIO pin initialization */
void gpio_motor_init(void)
{
    gpio_config_t gpio_config_motor = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_DEF_OUTPUT,
        .pin_bit_mask = (1ULL << GPIO_MOTOR_IN1) | (1ULL << GPIO_MOTOR_IN2),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&gpio_config_motor));
}

/* Main task for control motors */
void motor_task(void* pvParameters)
{
    EventBits_t bits;
    while(1)
    {
        bits = xEventGroupWaitBits(e_tasks, MQTT_GOT_MOTOR_STOP | MQTT_GOT_MOTOR_IN1_GO | MQTT_GOT_MOTOR_IN2_GO, pdTRUE, pdFALSE, portMAX_DELAY);

        if(bits & MQTT_GOT_MOTOR_STOP)
        {
            gpio_set_level(GPIO_MOTOR_IN1, 0);
            gpio_set_level(GPIO_MOTOR_IN2, 0);
        }
        else if(bits & MQTT_GOT_MOTOR_IN1_GO)
        {
            gpio_set_level(GPIO_MOTOR_IN1, 1);
            gpio_set_level(GPIO_MOTOR_IN2, 0);
        }
        else if(bits & MQTT_GOT_MOTOR_IN2_GO)
        {
            gpio_set_level(GPIO_MOTOR_IN1, 0);
            gpio_set_level(GPIO_MOTOR_IN2, 1);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}