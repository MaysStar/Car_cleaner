#ifndef COMMON_H
#define COMMON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include <time.h>

#define WIFI_BIT_GOT_IP         BIT0
#define MQTT_GOT_UPDATE         BIT1
#define DISTANCE_IN_DIAPASONE   BIT2
#define DISTANCE_OUT_DIAPASONE  BIT3        
#define SYNC_GOT_TIME           BIT4
#define OTA_BIT_IN_PROGRESS     BIT5
#define MQTT_SUBSCRIBE          BIT6

extern QueueHandle_t q_ota_level;
extern QueueHandle_t q_distance;
extern QueueHandle_t q_distance_pid;
extern QueueHandle_t q_servo_angle;
extern QueueHandle_t q_time;
extern QueueHandle_t q_angle;

extern EventGroupHandle_t e_tasks;

extern volatile time_t now;

extern SemaphoreHandle_t m_I2C0;
extern SemaphoreHandle_t m_I2C1;
extern SemaphoreHandle_t m_UART1;

extern portMUX_TYPE m_TIME;

#endif // COMMON_H