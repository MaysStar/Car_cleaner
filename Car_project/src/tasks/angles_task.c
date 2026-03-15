#include "angles_task.h"

static i2c_master_dev_handle_t i2c_master_dev_handle;

/* Gyroscope and accelerometer initialization */
void gyroscope_accelerometer_init()
{
    static i2c_master_bus_handle_t i2c_master_bus_handle;

    /* initialization data for MPU-6050 */
    static uint8_t init_buf1[2] = {
        0x6B, 0x01, // PWR_MGMT_1 PLL with X axis gyroscope reference
    };
    static uint8_t init_buf2[2] = {
        0x1B, 0x00, // GYRO_CONFIG Full Scale Range ± 250 °/s
    };
    static uint8_t init_buf3[2] = {
         0x1C, 0x00, // ACCEL_CONFIG Full Scale Range ± 2g
    };
    static uint8_t dlpf_config[2] = {0x1A, 0x04}; // DLPF = 20Hz

    i2c_master_bus_config_t i2c_master_bus_config = 
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_DEFAULT_PORT_MPU6050,
        .scl_io_num = I2C_SCL_PIN_MPU6050,
        .sda_io_num = I2C_SDA_PIN_MPU6050,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_bus_config, &i2c_master_bus_handle));

    i2c_device_config_t i2c_device_config = 
    {
        .scl_speed_hz = I2C_SPEED,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = I2C_SLAVE_ADDR_MPU6050,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master_bus_handle, &i2c_device_config, &i2c_master_dev_handle));

    xSemaphoreTake(m_I2C1, portMAX_DELAY);
    ESP_ERROR_CHECK(i2c_master_transmit(i2c_master_dev_handle, init_buf1, sizeof(init_buf1), portMAX_DELAY));
    ESP_ERROR_CHECK(i2c_master_transmit(i2c_master_dev_handle, init_buf2, sizeof(init_buf2), portMAX_DELAY));
    ESP_ERROR_CHECK(i2c_master_transmit(i2c_master_dev_handle, init_buf3, sizeof(init_buf3), portMAX_DELAY));
    ESP_ERROR_CHECK(i2c_master_transmit(i2c_master_dev_handle, dlpf_config, sizeof(dlpf_config), portMAX_DELAY));
    xSemaphoreGive(m_I2C1);
}

/* Main task for mpu-6050 */
void gyroscope_accelerometer_task(void* pvParameters)
{
    static uint8_t data_reg_addr = 0x3B;
    static uint8_t g_z_reg_addr = 0x47;
    static uint8_t rx_buf[I2C_MPU6050_DATA_SIZE] = {};
    static MPU6050_data_t MPU6050_data = {};

    static float real_x = 0.0f;
    static float real_y = 0.0f;
    static float real_z = 0.0f;

    static float g_z_error;

    static const float alpha = 0.98f;
    static int64_t integration_time_now;
    static int64_t integration_time_prev = 0;
    static char angle_char_buf[64] = {};

    /* Z (yaw) calibration ( calculate static arror )*/

    int64_t error_z_sum = 0;
    const int CALIBRATION_SAMPLES = 200;

    for(uint32_t i = 0; i < CALIBRATION_SAMPLES; ++i)
    {
        uint8_t g_z_byte[2];
        xSemaphoreTake(m_I2C1, portMAX_DELAY);
        // get value from refister GYRO_ZOUT_H to GYRO_ZOUT_L (2 baits)
        esp_err_t err = i2c_master_transmit_receive(i2c_master_dev_handle, &g_z_reg_addr, sizeof(g_z_reg_addr), g_z_byte, sizeof(g_z_byte), portMAX_DELAY);
        xSemaphoreGive(m_I2C1);

        if (err != ESP_OK) {
            ESP_LOGW("I2C", "some MPU-6050 error");
            // pressed 
            continue;
        }

        error_z_sum += (int16_t)((g_z_byte[0] << 8) | g_z_byte[1]);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    g_z_error = ((float)error_z_sum / (float)CALIBRATION_SAMPLES);

    integration_time_prev = esp_timer_get_time();

    while(1)
    {
        xSemaphoreTake(m_I2C1, portMAX_DELAY);
        // get value from refister ACCEL_XOUT_H to GYRO_ZOUT_L (14 baits)
        esp_err_t err = i2c_master_transmit_receive(i2c_master_dev_handle, &data_reg_addr, sizeof(data_reg_addr), rx_buf, sizeof(rx_buf), portMAX_DELAY);
        xSemaphoreGive(m_I2C1);

        if (err != ESP_OK) {
            ESP_LOGW("I2C", "some MPU-6050 error");
            // pressed 
            continue;
        }

        // drain baits
        MPU6050_data.accel_x = (int16_t)((rx_buf[0] << 8) | rx_buf[1]);
        MPU6050_data.accel_y = (int16_t)((rx_buf[2] << 8) | rx_buf[3]);
        MPU6050_data.accel_z = (int16_t)((rx_buf[4] << 8) | rx_buf[5]);
        MPU6050_data.temp    = (int16_t)((rx_buf[6] << 8) | rx_buf[7]);
        MPU6050_data.gyro_x  = (int16_t)((rx_buf[8] << 8) | rx_buf[9]);
        MPU6050_data.gyro_y  = (int16_t)((rx_buf[10] << 8) | rx_buf[11]);
        MPU6050_data.gyro_z  = (int16_t)((rx_buf[12] << 8) | rx_buf[13]);

        // calculate accurate value of angle (Complementary filter)
        integration_time_now = esp_timer_get_time();
        
        float dt = (float)(integration_time_now - integration_time_prev) / 1000000.0f;

        integration_time_prev = integration_time_now;

        // Normalize data 
        float gx = ((float)MPU6050_data.gyro_x) / 131.0f;
        float gy = ((float)MPU6050_data.gyro_y) / 131.0f;
        float gz = ((float)(MPU6050_data.gyro_z - g_z_error) / 131.0f);

        float ax = (float)MPU6050_data.accel_x / 16384.0f;
        float ay = (float)MPU6050_data.accel_y / 16384.0f;
        float az = (float)MPU6050_data.accel_z / 16384.0f;

        /* filter dynamic gyro error if angle speed less than 0.5 plus or minus */
        if(fabs(gx) < 0.5f)
        {
            gx = 0.0f;
        }

        if(fabs(gy) < 0.5f)
        {
            gy = 0.0f;
        }

        if(fabs(gz) < 0.5f)
        {
            gz = 0.0f;
        }

        /* Calculate real accelerometer value */        
        float accel_angle_roll  = atan2f(ay, az) * RAD_TO_DEG;
        float accel_angle_pitch = atan2f(ax, az) * RAD_TO_DEG;

        /* First initialization for fast start */
        static bool first_run = true;
        if (first_run)
        {
            real_x = accel_angle_roll;
            real_y = accel_angle_pitch;
            first_run = false;
        }

        // Complementary filter
        real_x = alpha * (real_x + gx * dt) + (1.0f - alpha) * accel_angle_roll;
        real_y = alpha * (real_y + gy * dt) + (1.0f - alpha) * accel_angle_pitch;
        real_z = real_z + gz * dt;
 
        snprintf(angle_char_buf, sizeof(angle_char_buf), "x:%3.1f y:%3.1f z:%3.1f", real_x, real_y, real_z);
        xQueueSend(q_angle, angle_char_buf, pdMS_TO_TICKS(10));

        vTaskDelay(1);
    }
}