#ifndef DISTANCE_TASK_H
#define DISTANCE_TASK_H

#include "stdio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "common.h"

#define UART_DEFAULT_PORT UART_NUM_1

#define UART_TX_PIN GPIO_NUM_17
#define UART_RX_PIN GPIO_NUM_18

extern QueueHandle_t q_distance;

void uart_init(void);
void uart_task(void* pvParameters);

#endif // DISTANCE_TASK_H