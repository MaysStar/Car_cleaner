#include "stdio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_ota_ops.h"
#include "esp_psram.h"
#include "esp_app_desc.h"

#include "motor_control_task.h"
#include "wifi_task.h"
#include "mqtts_hiveMQ_task.h"
#include "ota_task.h"
#include "distance_task.h"
#include "oled_task.h"
#include "common.h"

TaskHandle_t motor_task_handle = NULL;
TaskHandle_t wifi_task_handle = NULL;
TaskHandle_t ota_task_handle = NULL;
TaskHandle_t uart_task_handle = NULL;
TaskHandle_t oled_task_handle = NULL;
TaskHandle_t mqtts_hiveMQ_task_handle = NULL;

QueueHandle_t q_distance = NULL;
EventGroupHandle_t e_tasks = NULL;

void app_main() 
{
    gpio_motor_init();
    uart_init();
    oled_init();
    q_distance = xQueueCreate(10, sizeof(float));
    configASSERT(q_distance != NULL);

    e_tasks = xEventGroupCreate();
    configASSERT(e_tasks != NULL);

    xTaskCreate(motor_task, "motor_task", 4096, NULL, 5, &motor_task_handle);
    configASSERT(motor_task_handle != NULL);

    xTaskCreate(wifi_task, "wifi_task", 8192, NULL, 5, &wifi_task_handle);
    configASSERT(wifi_task_handle != NULL);

    xTaskCreate(mqtts_hiveMQ_task, "mqtts_hiveMQ_task", 8192, NULL, 5, &mqtts_hiveMQ_task_handle);
    configASSERT(mqtts_hiveMQ_task_handle != NULL);

    xTaskCreate(ota_task, "ota_task", 12288, NULL, 5, &ota_task_handle);
    configASSERT(ota_task_handle != NULL);

    xTaskCreate(uart_task, "uart_task", 4096, NULL, 5, &uart_task_handle);
    configASSERT(uart_task_handle != NULL);

    xTaskCreate(oled_task, "oled_task", 4096, NULL, 5, &oled_task_handle);
    configASSERT(oled_task_handle != NULL);
}