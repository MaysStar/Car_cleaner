#include "ota_task.h"

static const char* TAG = "ota_task";

/* Main task for ota */
void ota_task(void* pvParameters)
{
    static uint32_t update_size = 0;
    static uint32_t ota_percent = 0;
    while(1)
    {
        xEventGroupWaitBits(e_tasks, MQTT_GOT_UPDATE, pdTRUE, pdTRUE, portMAX_DELAY);

        esp_http_client_config_t esp_http_client_config = 
        {
            .url = CONFIG_OTA_HTTPS_URL_GITHUB,
            .crt_bundle_attach = esp_crt_bundle_attach,
            .timeout_ms = 10000,
            .keep_alive_enable = true,
            .buffer_size = 8192,
            .buffer_size_tx = 8192,
        };
        
        esp_https_ota_handle_t ota_handle = NULL;
        esp_https_ota_config_t ota_config = {
            .http_config = &esp_http_client_config,
        };

        ESP_LOGI(TAG, "Starting HTTPS OTA Update...");

        // Start preparation
        esp_err_t err = esp_https_ota_begin(&ota_config, &ota_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Begin error");
            goto cleanup;
        }

        xEventGroupSetBits(e_tasks, OTA_BIT_IN_PROGRESS);

        /* get size of all update */
        update_size = esp_https_ota_get_image_size(ota_handle);

        // perform OTA
        while(esp_https_ota_perform(ota_handle) == ESP_ERR_HTTPS_OTA_IN_PROGRESS)
        {
            ota_percent = (esp_https_ota_get_image_len_read(ota_handle) * 100) / update_size;
            xQueueSend(q_ota_level, &ota_percent, pdMS_TO_TICKS(10));
            vTaskDelay(1);
        }

        xEventGroupClearBits(e_tasks, OTA_BIT_IN_PROGRESS);

        // Finish update
        if (esp_https_ota_finish(ota_handle) == ESP_OK) {
            ESP_LOGI(TAG, "OTA finished, restarting...");
            esp_restart();
        }
        else 
        {
            ESP_LOGE(TAG, "OTA failed!");
        }
cleanup:
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}