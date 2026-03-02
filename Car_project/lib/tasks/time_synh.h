#ifndef TIME_SYNH_H
#define TIME_SYNH_H

#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"

#include <time.h>
#include <string.h>

#include "common.h"
#include "timer.h"

void time_sync(void);

#endif // TIME_SYNH_H