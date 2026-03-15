#include "wifi.h"

static const char* TAG = "wifi";

/* Wi-fi initialize */
void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //esp_netif_t* esp_netif = 
    esp_netif_create_default_wifi_sta();

    ESP_ERROR_CHECK(esp_netif_init());

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config = 
    {
        .sta.ssid = CONFIG_WIFI_SSID_LVIV,
        .sta.password = CONFIG_WIFI_PASSWORD_LVIV,
        .sta.threshold.authmode = WIFI_AUTH_WPA2_PSK,
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_start());
}

void wifi_handler(  void* event_handler_arg,
                    esp_event_base_t event_base,
                    int32_t event_id,
                    void* event_data)
{
    if(event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
                ESP_ERROR_CHECK(esp_wifi_connect());
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                esp_err_t ret;
                ESP_LOGE(TAG, "WIFI_EVENT_STA_DISCONNECTED");
                ret = esp_wifi_connect();
                if (ret != ESP_OK && ret != ESP_ERR_WIFI_NOT_STARTED) {
                    ESP_LOGE("WIFI", "Connection failed: %d", ret);
                }
                break;
            default:
                break;
        }
    }
    else if(event_base == IP_EVENT)
    {
        switch (event_id)
        {
            case IP_EVENT_STA_GOT_IP:
                ip_event_got_ip_t* ip_data = (ip_event_got_ip_t*)event_data;
                ESP_LOGI(TAG, "IP which we got:"IPSTR, IP2STR(&ip_data->ip_info.ip));

                ESP_ERROR_CHECK(esp_ota_mark_app_valid_cancel_rollback());

                xEventGroupSetBits(e_tasks, WIFI_BIT_GOT_IP);

                break;
            case IP_EVENT_STA_LOST_IP:
                ESP_LOGE(TAG, "IP_EVENT_STA_LOST_IP");
                break;
            default:
                break;
        }
    }
}