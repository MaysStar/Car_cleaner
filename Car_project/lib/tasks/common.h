#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define WIFI_BIT_GOT_IP         BIT0
#define MQTT_GOT_UPDATE         BIT1
#define MQTT_GOT_MOTOR_STOP     BIT2
#define MQTT_GOT_MOTOR_IN1_GO   BIT3   
#define MQTT_GOT_MOTOR_IN2_GO   BIT4   

extern QueueHandle_t q_distance;

extern EventGroupHandle_t e_tasks;