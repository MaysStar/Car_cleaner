#include "mqtts_hiveMQ_task.h"

static const char* TAG = "mqtts_haveMQ_task";
static char rx_mqtts_buf[256];
static char topic[128];

/* time sync for mqtts connection */
void time_sync(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");

    // get time from server periodically
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);

    // set server
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);

    // try sync time
    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(20000)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update system time within 20s timeout");
    }

    // wait for time stability
    vTaskDelay(pdMS_TO_TICKS(2000));

    // time struct 
    time_t now;
    struct tm timeinfo;

    // get current time and format for common 
    time(&now);
    localtime_r(&now, &timeinfo);

    // print current time 
    ESP_LOGI(TAG, "Current time: %d-%d-%d %d:%d:%d", 
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

/* Main mqtts task with connected to hiveMQ */
void mqtts_hiveMQ_task(void* pvParameters)
{
    xEventGroupWaitBits(e_tasks, WIFI_BIT_GOT_IP, pdFALSE, pdTRUE, portMAX_DELAY);

    static esp_mqtt_client_config_t esp_mqtt_client_config = 
    {
        .broker.address.hostname = CONFIG_OTA_MQTTS_HOSTNAME,
        .broker.address.port = 8883,
        .broker.verification.skip_cert_common_name_check = false,
        .broker.address.transport = MQTT_TRANSPORT_OVER_SSL,
        .credentials.username = CONFIG_OTA_MQTTS_USERNAME,
        .credentials.authentication.password = CONFIG_OTA_MQTTS_PASSWORD,
    };

    esp_mqtt_client_config.broker.verification.certificate = hivemq_root_ca;
    esp_mqtt_client_config.broker.verification.certificate_len = strlen(hivemq_root_ca) + 1;

    time_sync();

    esp_mqtt_client_handle_t esp_mqtt_client_handle = esp_mqtt_client_init(&esp_mqtt_client_config);

    if(esp_mqtt_client_handle != NULL)
    {
        ESP_ERROR_CHECK(esp_mqtt_client_register_event(esp_mqtt_client_handle, MQTT_EVENT_ANY, mqtts_hiveMQ_handler, NULL));

        ESP_ERROR_CHECK(esp_mqtt_client_start(esp_mqtt_client_handle));
    }

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* mqtt handler for ota update */
void mqtts_hiveMQ_handler(  void* event_handler_arg,
                    esp_event_base_t event_base,
                    int32_t event_id,
                    void* event_data)
{
    esp_mqtt_event_t* esp_mqtt_event = (esp_mqtt_event_t*)event_data;
    switch (event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Connected to HiveMQ! Subscribing...");
            esp_mqtt_client_subscribe(esp_mqtt_event->client, "car/ota", 1);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT DATA");

            snprintf(topic, sizeof(topic), "%.*s", esp_mqtt_event->topic_len, esp_mqtt_event->topic);
            snprintf(rx_mqtts_buf, sizeof(rx_mqtts_buf), "%.*s", esp_mqtt_event->data_len, esp_mqtt_event->data);

            if(strcmp(topic, "car/ota") == 0)
            {
                if(strcmp(rx_mqtts_buf, "true") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_UPDATE);
            }
            
            else if(strcmp(topic, "car/motors") == 0)
            {
                if(strcmp(rx_mqtts_buf, "stop") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR_STOP);

                else if(strcmp(rx_mqtts_buf, "in1") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR_IN1_GO);

                else if(strcmp(rx_mqtts_buf, "in2") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR_IN2_GO);
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT Error occurred");
            break;
        default:
            break;
    }
}