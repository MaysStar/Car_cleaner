#include "ota_task.h"

static const char* TAG = "ota_task";

extern const char* server_cert_pem_start;

/* Main task for ota */
void ota_task(void* pvParameters)
{
    while(1)
    {
        xEventGroupWaitBits(e_tasks, MQTT_GOT_UPDATE, pdTRUE, pdTRUE, portMAX_DELAY);

        esp_http_client_config_t esp_http_client_config = 
        {
            .url = CONFIG_OTA_HTTPS_URL,
            .cert_pem = server_cert_pem_start,
            .timeout_ms = 10000, 
            .keep_alive_enable = true,
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

        // perform OTA
        while(esp_https_ota_perform(ota_handle) == ESP_ERR_HTTPS_OTA_IN_PROGRESS)
        {
            vTaskDelay(1);
        }

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