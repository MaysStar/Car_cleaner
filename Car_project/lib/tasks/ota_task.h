#ifndef OTA_TASK_H
#define OTA_TASK_H

#include "esp_err.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "esp_app_desc.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include "esp_crt_bundle.h"

#include <time.h>
#include <string.h>

#include "mqtt_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "config.h"
#include "common.h"
#include "certs.h"

void ota_task(void* pvParameters);
void mqtt_handler(  void* event_handler_arg,
                    esp_event_base_t event_base,
                    int32_t event_id,
                    void* event_data);
void time_sync(void);

#endif // OTA_TASK_H