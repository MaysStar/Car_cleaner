#include "stdio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_ota_ops.h"
#include "esp_psram.h"
#include "esp_app_desc.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "motor_control_task.h"
#include "servo_motor_task.h"
#include "wifi.h"
#include "mqtts_hiveMQ.h"
#include "ota_task.h"
#include "distance_task.h"
#include "oled_task.h"
#include "time_synh.h"
#include "angles_task.h"
#include "esp_logs_papertrail.h"

#include "common.h"

static char version[] = "Version: 1.5.1";

TaskHandle_t motor_task_handle = NULL;
TaskHandle_t ota_task_handle = NULL;
TaskHandle_t uart_task_handle = NULL;
TaskHandle_t oled_task_handle = NULL;
TaskHandle_t servo_motor_task_handle = NULL;
TaskHandle_t gyroscope_accelerometer_handle = NULL;
TaskHandle_t esp_logs_papertrail_handle = NULL;

QueueHandle_t q_ota_level = NULL;
QueueHandle_t q_distance = NULL;
QueueHandle_t q_distance_pid = NULL;
QueueHandle_t q_servo_angle = NULL;
QueueHandle_t q_time = NULL;
QueueHandle_t q_angle = NULL;
QueueHandle_t q_speed = NULL;

SemaphoreHandle_t m_I2C0;
SemaphoreHandle_t m_I2C1;
SemaphoreHandle_t m_UART1;

// special mutex for data which update throught gptimer isr
portMUX_TYPE m_TIME = portMUX_INITIALIZER_UNLOCKED;

EventGroupHandle_t e_tasks = NULL;

volatile time_t now = 0; // time struct 

void app_main() 
{
    /* initialization */
    esp_err_t ret = nvs_flash_init();

    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // if some problem with it we erase old flash and upload new
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    /* Esp reset error */

    /* First create all groups and queues */
    /* queue */
    q_ota_level = xQueueCreate(5, sizeof(int32_t));
    configASSERT(q_ota_level != NULL);

    q_speed = xQueueCreate(5, sizeof(int32_t));
    configASSERT(q_speed != NULL);

    q_distance = xQueueCreate(10, sizeof(float));
    configASSERT(q_distance != NULL);

    q_distance_pid = xQueueCreate(10, sizeof(float));
    configASSERT(q_distance_pid != NULL);

    q_servo_angle = xQueueCreate(5, sizeof(int32_t));
    configASSERT(q_servo_angle != NULL);

    q_time = xQueueCreate(5, sizeof(char) * 64);
    configASSERT(q_time != NULL);

    q_angle = xQueueCreate(5, sizeof(char) * 64);
    configASSERT(q_angle != NULL);

    /* mutex */
    m_I2C0 = xSemaphoreCreateMutex();
    configASSERT(m_I2C0 != NULL);

    m_I2C1 = xSemaphoreCreateMutex();
    configASSERT(m_I2C1 != NULL);

    m_UART1 = xSemaphoreCreateMutex();
    configASSERT(m_UART1 != NULL);

    /* event group */
    e_tasks = xEventGroupCreate();
    configASSERT(e_tasks != NULL);

    set_version(version);
    dc_motor_init();
    wifi_init();

    uart_init();
    //gyroscope_accelerometer_init();
    
    time_sync();
    mqtts_hiveMQ();
    oled_init();
    
    /* tasks */
    xTaskCreate(esp_logs_papertrail_task, "esp_logs_papertrail_task", 4096, NULL, 3, &esp_logs_papertrail_handle);
    configASSERT(esp_logs_papertrail_handle != NULL);

    xTaskCreate(ota_task, "ota_task", 12288, NULL, 5, &ota_task_handle);
    configASSERT(ota_task_handle != NULL);

    xTaskCreate(motor_task, "motor_task", 8192, NULL, 4, &motor_task_handle);
    configASSERT(motor_task_handle != NULL);

    xTaskCreate(servo_motor_task, "servo_motor_task", 4096, NULL, 4, &servo_motor_task_handle);
    configASSERT(servo_motor_task_handle != NULL);

    //xTaskCreate(gyroscope_accelerometer_task, "gyroscope_accelerometer_task", 4096, NULL, 4, &gyroscope_accelerometer_handle);
    //configASSERT(gyroscope_accelerometer_handle != NULL);

    xTaskCreate(uart_task, "uart_task", 4096, NULL, 3, &uart_task_handle);
    configASSERT(uart_task_handle != NULL);

    xTaskCreate(oled_task, "oled_task", 4096, NULL, 3, &oled_task_handle);
    configASSERT(oled_task_handle != NULL);
}