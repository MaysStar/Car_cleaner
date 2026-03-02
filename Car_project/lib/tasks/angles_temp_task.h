#ifndef ANGLES_TASK_H
#define ANGLES_TASK_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include "driver/i2c_master.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>

#include "common.h"

#define I2C_DEFAULT_PORT_MPU6050 I2C_NUM_1

#define I2C_SCL_PIN_MPU6050     GPIO_NUM_12
#define I2C_SDA_PIN_MPU6050     GPIO_NUM_11

#define I2C_SLAVE_ADDR_MPU6050  0x68
#define I2C_SPEED               100000

#define I2C_MPU6050_DATA_SIZE   14

#define RAD_TO_DEG 57.29578f

void gyroscope_accelerometer_init(void);
void gyroscope_accelerometer_task(void* pvParameters);

#endif // ANGLES_TASK_H