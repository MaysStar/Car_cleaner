#ifndef MQTTS_HIVEMQ_H
#define MQTTS_HIVEMQ_H

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "mqtt_client.h"

#include "certs.h"
#include "time.h"
#include "common.h"
#include "config.h"

void mqtts_hiveMQ(void);
void mqtts_hiveMQ_handler(  void* event_handler_arg,
                    esp_event_base_t event_base,
                    int32_t event_id,
                    void* event_data);
void time_sync(void);

#endif // MQTTS_HIVEMQ_H