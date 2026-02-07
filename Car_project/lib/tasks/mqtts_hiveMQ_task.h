#ifndef MQTTS_HIVEMQ_TASK_H
#define MQTTS_HIVEMQ_TASK_H

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "mqtt_client.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include <time.h>
#include <string.h>

#include "certs.h"
#include "common.h"

void mqtts_hiveMQ_task(void* pvParameters);
void mqtts_hiveMQ_handler(  void* event_handler_arg,
                    esp_event_base_t event_base,
                    int32_t event_id,
                    void* event_data);
void time_sync(void);

#endif // MQTTS_HIVEMQ_TASK_H