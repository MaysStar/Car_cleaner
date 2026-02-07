#ifndef MOTOR_CONTROL_TASK_H
#define MOTOR_CONTROL_TASK_H

#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

#include "common.h"

#define GPIO_MOTOR_IN1 GPIO_NUM_1
#define GPIO_MOTOR_IN2 GPIO_NUM_2

void gpio_motor_init(void);
void motor_task(void* pvParameters);

#endif // MOTOR_CONTROL_TASK_H