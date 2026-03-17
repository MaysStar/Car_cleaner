#ifndef ESP_LOGS_PAPERTRAIL_H 
#define ESP_LOGS_PAPERTRAIL_H

#include <stdio.h>

#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "config.h"
#include "common.h"

#define EARLY_LOG_SIZE     128
#define EARLY_LOG_BUF_SIZE 64

void esp_logs_papertrail_init(void);

int new_hook_papertrail_function(const char *frm, va_list list);
void esp_logs_papertrail_task(void* pvParameters);

#endif // ESP_LOGS_PAPERTRAIL_H