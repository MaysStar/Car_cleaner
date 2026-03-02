#include "motor_control_task.h"

static const char* TAG = "motor_control_task";

/* GPIO pin initialization */
void gpio_motor_init(void)
{
    gpio_config_t gpio_config_motor = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_DEF_OUTPUT,
        .pin_bit_mask = (1ULL << GPIO_MOTOR_IN1) | (1ULL << GPIO_MOTOR_IN2) | (1ULL << GPIO_MOTOR_IN3) | (1ULL << GPIO_MOTOR_IN4),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&gpio_config_motor));
}

/* Main task for control motors */
void motor_task(void* pvParameters)
{
    EventBits_t bits;
    while(1)
    {
        bits = xEventGroupWaitBits(e_tasks, MQTT_GOT_MOTOR_START | 
                                            MQTT_GOT_MOTOR_STOP | 
                                            MQTT_GOT_MOTOR1_START | 
                                            MQTT_GOT_MOTOR2_START | 
                                            MQTT_GOT_MOTOR1_BACK | 
                                            MQTT_GOT_MOTOR2_BACK |
                                            DISTANCE_LESS_THEN_20CM |
                                            DISTANCE_MORE_THEN_20CM, pdTRUE, pdFALSE, portMAX_DELAY);
        if(bits & DISTANCE_LESS_THEN_20CM)
        {
            gpio_set_level(GPIO_MOTOR_IN1, 0);
            gpio_set_level(GPIO_MOTOR_IN2, 0);
            gpio_set_level(GPIO_MOTOR_IN3, 0);
            gpio_set_level(GPIO_MOTOR_IN4, 0);
            ESP_LOGI(TAG, "All Motors Stop");
            continue;
        }
        else if(bits & DISTANCE_MORE_THEN_20CM)
        {
            gpio_set_level(GPIO_MOTOR_IN1, 1); gpio_set_level(GPIO_MOTOR_IN2, 0);
            gpio_set_level(GPIO_MOTOR_IN4, 1); gpio_set_level(GPIO_MOTOR_IN3, 0);\
            ESP_LOGI(TAG, "All Motors Forward");
        }

        if(bits & MQTT_GOT_MOTOR_STOP)
        {
            gpio_set_level(GPIO_MOTOR_IN1, 0);
            gpio_set_level(GPIO_MOTOR_IN2, 0);
            gpio_set_level(GPIO_MOTOR_IN3, 0);
            gpio_set_level(GPIO_MOTOR_IN4, 0);
            ESP_LOGI(TAG, "All Motors Stop");
            continue;
        }

        if(bits & MQTT_GOT_MOTOR_START)
        {
            gpio_set_level(GPIO_MOTOR_IN1, 1); gpio_set_level(GPIO_MOTOR_IN2, 0);
            gpio_set_level(GPIO_MOTOR_IN4, 1); gpio_set_level(GPIO_MOTOR_IN3, 0);
            ESP_LOGI(TAG, "All Motors Forward");
        }

        else if(bits & MQTT_GOT_MOTOR_BACK)
        {
            gpio_set_level(GPIO_MOTOR_IN1, 0); gpio_set_level(GPIO_MOTOR_IN2, 1);
            gpio_set_level(GPIO_MOTOR_IN4, 0); gpio_set_level(GPIO_MOTOR_IN3, 1);
            ESP_LOGI(TAG, "All Motors Forward");
        }

        // (IN1, IN2)
        if(bits & MQTT_GOT_MOTOR1_START) 
        {
            gpio_set_level(GPIO_MOTOR_IN1, 1);
            gpio_set_level(GPIO_MOTOR_IN2, 0);
            ESP_LOGI(TAG, "Motor 1 Forward");
        } 
        else if(bits & MQTT_GOT_MOTOR1_BACK) 
        {
            gpio_set_level(GPIO_MOTOR_IN1, 0);
            gpio_set_level(GPIO_MOTOR_IN2, 1);
            ESP_LOGI(TAG, "Motor 1 Backward");
        }

        // (IN3, IN4)
        if(bits & MQTT_GOT_MOTOR2_START) 
        {
            gpio_set_level(GPIO_MOTOR_IN3, 0);
            gpio_set_level(GPIO_MOTOR_IN4, 1);
            ESP_LOGI(TAG, "Motor 2 Forward");
        } 
        else if(bits & MQTT_GOT_MOTOR2_BACK) 
        {
            gpio_set_level(GPIO_MOTOR_IN3, 1);
            gpio_set_level(GPIO_MOTOR_IN4, 0);
            ESP_LOGI(TAG, "Motor 2 Backward");
        }
    }
}