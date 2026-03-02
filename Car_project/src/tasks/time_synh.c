#include "time_synh.h"

static const char* TAG = "time";

/* time sync */
void time_sync(void)
{
    xEventGroupWaitBits(e_tasks, WIFI_BIT_GOT_IP, pdFALSE, pdTRUE, portMAX_DELAY);

    ESP_LOGI(TAG, "Initializing SNTP");

    // get time from server periodically
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);

    // set server
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    ESP_ERROR_CHECK(esp_netif_sntp_init(&config));

    // try sync time
    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(20000)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update system time within 20s timeout");
    }

    // wait for time stability
    vTaskDelay(pdMS_TO_TICKS(2000));

    // get current time and format for common 
    time(&now);

    /* set time zone */
    ESP_ERROR_CHECK(setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1));
    tzset();

    /* wait until now will free from function time */
    vTaskDelay(pdMS_TO_TICKS(1000));

    xEventGroupSetBits(e_tasks, SYNC_GOT_TIME);

    timer_init();    
}