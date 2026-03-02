#ifndef COMMON_H
#define COMMON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include <time.h>

#define WIFI_BIT_GOT_IP         BIT0
#define MQTT_GOT_UPDATE         BIT1
#define MQTT_GOT_MOTOR_STOP     BIT2
#define MQTT_GOT_MOTOR_IN1_GO   BIT3   
#define MQTT_GOT_MOTOR_IN2_GO   BIT4   
#define MQTT_GOT_MOTOR_START    BIT5
#define DISTANCE_LESS_THEN_20CM BIT6
#define DISTANCE_MORE_THEN_20CM BIT7        
#define SYNC_GOT_TIME           BIT8
#define OTA_BIT_IN_PROGRESS     BIT9
#define MQTT_SUBSCRIBE          BIT10
#define MQTT_GOT_MOTOR1_START   BIT11
#define MQTT_GOT_MOTOR2_START   BIT12
#define MQTT_GOT_MOTOR1_BACK    BIT13
#define MQTT_GOT_MOTOR2_BACK    BIT14
#define MQTT_GOT_MOTOR_BACK     BIT15

typedef struct 
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temp;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} MPU6050_data_t;

extern QueueHandle_t q_ota_level;
extern QueueHandle_t q_distance;
extern QueueHandle_t q_servo_angle;
extern QueueHandle_t q_time;
extern QueueHandle_t q_angle_temp;

extern EventGroupHandle_t e_tasks;

extern volatile time_t now;

extern SemaphoreHandle_t m_I2C0;
extern SemaphoreHandle_t m_I2C1;
extern SemaphoreHandle_t m_UART1;

extern portMUX_TYPE m_TIME;

#endif // COMMON_H