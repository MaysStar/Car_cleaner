#include "mqtts_hiveMQ.h"

static const char* TAG = "mqtts_haveMQ";

static char rx_mqtts_buf[256];
static char topic_buf[128];
static int32_t servo_angle;

/* Main mqtts function which connect to hiveMQ */
void mqtts_hiveMQ(void)
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

    esp_mqtt_client_handle_t esp_mqtt_client_handle = esp_mqtt_client_init(&esp_mqtt_client_config);

    if(esp_mqtt_client_handle != NULL)
    {
        ESP_ERROR_CHECK(esp_mqtt_client_register_event(esp_mqtt_client_handle, MQTT_EVENT_ANY, mqtts_hiveMQ_handler, NULL));

        ESP_ERROR_CHECK(esp_mqtt_client_start(esp_mqtt_client_handle));
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
            esp_mqtt_client_subscribe(esp_mqtt_event->client, "car/motors", 1);
            esp_mqtt_client_subscribe(esp_mqtt_event->client, "car/servo", 1);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT DATA");

            snprintf(topic_buf, sizeof(topic_buf), "%.*s", esp_mqtt_event->topic_len, esp_mqtt_event->topic);
            snprintf(rx_mqtts_buf, sizeof(rx_mqtts_buf), "%.*s", esp_mqtt_event->data_len, esp_mqtt_event->data);

            if(strcmp(topic_buf, "car/ota") == 0)
            {
                ESP_LOGI(TAG, "car/ota");
                if(strcmp(rx_mqtts_buf, "true") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_UPDATE);
            }
            
            else if(strcmp(topic_buf, "car/motors") == 0)
            {
                ESP_LOGI(TAG, "car/motors");
                if(strcmp(rx_mqtts_buf, "stop") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR_STOP);

                else if(strcmp(rx_mqtts_buf, "in1") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR_IN1_GO);

                else if(strcmp(rx_mqtts_buf, "in2") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR_IN2_GO);

                else if(strcmp(rx_mqtts_buf, "start") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR_START);

                else if(strcmp(rx_mqtts_buf, "back") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR_START);

                else if(strcmp(rx_mqtts_buf, "start1") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR1_START);

                else if(strcmp(rx_mqtts_buf, "start2") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR2_START);

                else if(strcmp(rx_mqtts_buf, "back1") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR1_BACK);
                
                else if(strcmp(rx_mqtts_buf, "back2") == 0) xEventGroupSetBits(e_tasks, MQTT_GOT_MOTOR2_BACK);
            }

            else if(strcmp(topic_buf, "car/servo") == 0)
            {   
                ESP_LOGI(TAG, "car/servo");
                servo_angle = atoi(rx_mqtts_buf);
                if(servo_angle > 180) servo_angle = 180;
                else if(servo_angle < 0) servo_angle = 0;
                xQueueSend(q_servo_angle, &servo_angle, 0);
            }
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MESSAGES SUBSCRIBED SUCCESSFULLY, msg_id=%d", esp_mqtt_event->msg_id);
            xEventGroupSetBits(e_tasks, MQTT_SUBSCRIBE);
            break;

        case MQTT_EVENT_ERROR:
            xEventGroupClearBits(e_tasks, MQTT_SUBSCRIBE);
            ESP_LOGE(TAG, "MQTT Error occurred");
            break;
        default:
            break;
    }
}