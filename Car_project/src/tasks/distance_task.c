#include "distance_task.h"

static const char* TAG = "UART_TASK";

/* UART initialize for distance measurement */
void uart_init(void)
{
    // Set driver for uart 
    ESP_ERROR_CHECK(uart_driver_install(UART_DEFAULT_PORT, 2048, 0, 0, NULL, 0));

    // Set uart configuration
    uart_config_t uart_config = 
    {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .parity = UART_PARITY_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
        .stop_bits = UART_STOP_BITS_1,
    };

    // Config uart parameters
    ESP_ERROR_CHECK(uart_param_config(UART_DEFAULT_PORT, &uart_config));

    // Set uart pins
    ESP_ERROR_CHECK(uart_set_pin(UART_DEFAULT_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

/* Main UART & measurement distance task */
void uart_task(void* pvParameters)
{
    static uint8_t rx_buf[2];
    static uint16_t distance_mm = 0;
    while(1)
    {
        ESP_ERROR_CHECK(uart_flush(UART_DEFAULT_PORT));
        uart_write_bytes(UART_DEFAULT_PORT, "U", 1);

        vTaskDelay(pdMS_TO_TICKS(30));

        //  Read 2 bytes
        int len = uart_read_bytes(UART_DEFAULT_PORT, rx_buf, 2, pdMS_TO_TICKS(100));

        if(len == 2)
        {
            // Create distance in mm
            distance_mm = (uint16_t)(rx_buf[0] << 8 | rx_buf[1]);

            float distance_cm = (float)(distance_mm) / 10.0f;
            
            //ESP_LOGI(TAG, "Distance: %f cm\n", distance_cm);

            xQueueSend(q_distance, &distance_cm, portMAX_DELAY);
        }
        else  ESP_LOGE(TAG, "Can't measure distance");
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
