#ifndef MOTOR_CONTROL_TASK_H
#define MOTOR_CONTROL_TASK_H

#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

#include "driver/mcpwm_prelude.h"
#include "esp_timer.h"

#include "common.h"

#define GPIO_MOTOR_IN1 GPIO_NUM_1
#define GPIO_MOTOR_IN2 GPIO_NUM_2

#define GPIO_MOTOR_IN3 GPIO_NUM_4
#define GPIO_MOTOR_IN4 GPIO_NUM_5

#define MOTOR_POWERS   200

typedef enum{
    MOTORS_FORWARD_BACKWARD = 0,
    MOTORS_ROTATE,
}Motors_state_t;

typedef struct 
{
    /* Controler constant */
    float Kp;
    float Ki;
    float Kd;

    /* Time */
    uint64_t prev_time;
    uint64_t curr_time;

    /* Controler memory */
    float prev_error;
    float prev_measurement;
    float tau;

    /* Calculated value */
    float proportional;
    float integrator;
    float differentiator;

    /* Limitatiaon */
    float min_result;
    float max_result;

    /* Result */
    float out;
}PID_controler_t;


void dc_motor_init(void);

int32_t percent_into_ticks(int32_t percent_in_255);
void motors_control(int32_t percent_in_255);

void pid_controler_init(PID_controler_t* PID_controler);
int32_t pid_controler_update(PID_controler_t* PID_controler, float distance_cm, float setpoint);

void motor_task(void* pvParameters);

#endif // MOTOR_CONTROL_TASK_H