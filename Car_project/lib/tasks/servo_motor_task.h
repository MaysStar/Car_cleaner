#ifndef SERVO_MOTOR_TASK_H
#define SERVO_MOTOR_TASK_H

#include "stdio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "driver/mcpwm_prelude.h"

#include "common.h"

#define GPIO_SERVO_MOTOR GPIO_NUM_3

#define SERVO_USE_MQTTS_COMMAND 0
#define SERVO_USE_ORDINARY_MODE 1

void servo_motor_init(void);
void set_angle(int32_t angle);
void servo_motor_task(void* pvParameters);

#endif // SERVO_MOTOR_TASK_H