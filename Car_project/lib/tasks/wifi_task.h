#ifndef WIFI_TASK_H
#define WIFI_TASK_H

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_ota_ops.h"

#include "common.h"

void wifi_init(void);
void wifi_task(void *pvParameters);
void wifi_handler(  void* event_handler_arg,
                    esp_event_base_t event_base,
                    int32_t event_id,
                    void* event_data);

#endif // WIFI_TASK_H