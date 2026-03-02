#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"

#include "driver/gptimer.h"

#include <time.h>
#include <string.h>

#include "common.h"

void timer_init(void);

#endif // TIMER_H