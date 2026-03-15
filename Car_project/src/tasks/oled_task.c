 #include "oled_task.h"

static i2c_master_dev_handle_t i2c_master_dev_handle;
static uint8_t oled_full_buffer[OLED_SIZE + 1];
static uint32_t oled_full_buffer_idx = 1;
static void clear_buffer(void);
static char version_on_screen[32];

/* Function to set version on screen */
void set_version(const char* version)
{
    if(version != NULL)
    {
        strncpy(version_on_screen, version, sizeof(version_on_screen) - 1);
        version_on_screen[sizeof(version_on_screen) - 1] = '\0';
    }
}

/* Initialize Oled display*/
void oled_init(void)
{
    i2c_master_bus_config_t i2c_master_bus_config = 
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_DEFAULT_PORT,
        .scl_io_num = I2C_SCL_PIN,
        .sda_io_num = I2C_SDA_PIN,
    };

    i2c_master_bus_handle_t i2c_master_bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_bus_config, &i2c_master_bus_handle));

    i2c_device_config_t i2c_device_config = 
    {
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = I2C_SLAVE_ADDR,
        .scl_speed_hz = I2C_SPEED_OLED,
        .scl_wait_us = I2C_SCL_WAIT_US,
    };
    
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master_bus_handle, &i2c_device_config, &i2c_master_dev_handle));

    // wait for stable initialization
    vTaskDelay(pdMS_TO_TICKS(100));

    uint8_t init_cmd[] = 
    {
        I2C_COMMAND_COMMAND_BYTE,               
        0xAE,               // Display OFF)
        0xD5, 0x80,         // 
        0xA8, 0x3F,         // Multiplex ratio (64 lines)
        0xD3, 0x00,         // Display offset 0
        0x40,               // Start line 0
        0x8D, 0x14,         // Charge Pump Enable
        0x20, 0x00,         // Horizontal Addressing Mode
        0xA1,               // Segment Remap
        0xC8,               // COM Scan Direction
        0xDA, 0x12,         // COM pins hardware config
        0x81, 0xCF,         // (Contrast)
        0xA4,               // display RAM content
        0xA6,               // Normal routine
        0xAF                // (Display ON)
    };

    xSemaphoreTake(m_I2C0, portMAX_DELAY);
    i2c_master_transmit(i2c_master_dev_handle, init_cmd, sizeof(init_cmd), I2C_TIMEOUT);
    xSemaphoreGive(m_I2C0);
    
    clear_buffer();
    xSemaphoreTake(m_I2C0, portMAX_DELAY);
    i2c_master_transmit(i2c_master_dev_handle, oled_full_buffer, OLED_SIZE + 1, I2C_TIMEOUT);
    xSemaphoreGive(m_I2C0);
}

// map for change symbol fomat for oled 128 x 64 display
const char_map_t font5x7[] = 
{
    {' ', {0x00, 0x00, 0x00, 0x00, 0x00}},
    {'.', {0x00, 0x60, 0x60, 0x00, 0x00}},
    {':', {0x00, 0x36, 0x36, 0x00, 0x00}},
    {'%', {0x23, 0x13, 0x08, 0x64, 0x62}},
    {'0', {0x3e, 0x51, 0x49, 0x45, 0x3e}},
    {'1', {0x00, 0x42, 0x7f, 0x40, 0x00}},
    {'2', {0x42, 0x61, 0x51, 0x49, 0x46}},
    {'3', {0x21, 0x41, 0x45, 0x4b, 0x31}},
    {'4', {0x18, 0x14, 0x12, 0x7f, 0x10}},
    {'5', {0x27, 0x45, 0x45, 0x45, 0x39}},
    {'6', {0x3c, 0x4a, 0x49, 0x49, 0x30}},
    {'7', {0x01, 0x71, 0x09, 0x05, 0x03}},
    {'8', {0x36, 0x49, 0x49, 0x49, 0x36}},
    {'9', {0x06, 0x49, 0x49, 0x29, 0x1e}},
    {'A', {0x7e, 0x11, 0x11, 0x11, 0x7e}},
    {'B', {0x7f, 0x49, 0x49, 0x49, 0x36}},
    {'C', {0x3e, 0x41, 0x41, 0x41, 0x22}},
    {'D', {0x7f, 0x41, 0x41, 0x22, 0x1c}},
    {'E', {0x7f, 0x49, 0x49, 0x49, 0x41}},
    {'F', {0x7f, 0x09, 0x09, 0x09, 0x01}},
    {'G', {0x3e, 0x41, 0x49, 0x49, 0x7a}},
    {'H', {0x7f, 0x08, 0x08, 0x08, 0x7f}},
    {'I', {0x00, 0x41, 0x7f, 0x41, 0x00}},
    {'J', {0x20, 0x40, 0x41, 0x3f, 0x01}},
    {'K', {0x7f, 0x08, 0x14, 0x22, 0x41}},
    {'L', {0x7f, 0x40, 0x40, 0x40, 0x40}},
    {'M', {0x7f, 0x02, 0x0c, 0x02, 0x7f}},
    {'N', {0x7f, 0x04, 0x08, 0x10, 0x7f}},
    {'O', {0x3e, 0x41, 0x41, 0x41, 0x3e}},
    {'P', {0x7f, 0x09, 0x09, 0x09, 0x06}},
    {'Q', {0x3e, 0x41, 0x51, 0x21, 0x5e}},
    {'R', {0x7f, 0x09, 0x19, 0x29, 0x46}},
    {'S', {0x46, 0x49, 0x49, 0x49, 0x31}},
    {'T', {0x01, 0x01, 0x7f, 0x01, 0x01}},
    {'U', {0x3f, 0x40, 0x40, 0x40, 0x3f}},
    {'V', {0x1f, 0x20, 0x40, 0x20, 0x1f}},
    {'W', {0x3f, 0x40, 0x38, 0x40, 0x3f}},
    {'X', {0x63, 0x14, 0x08, 0x14, 0x63}},
    {'Y', {0x07, 0x08, 0x70, 0x08, 0x07}},
    {'Z', {0x61, 0x51, 0x49, 0x45, 0x43}},
    {'a', {0x20, 0x54, 0x54, 0x54, 0x78}},
    {'b', {0x7f, 0x48, 0x44, 0x44, 0x38}},
    {'c', {0x38, 0x44, 0x44, 0x44, 0x20}},
    {'d', {0x38, 0x44, 0x44, 0x48, 0x7f}},
    {'e', {0x38, 0x54, 0x54, 0x54, 0x18}},
    {'f', {0x08, 0x7e, 0x09, 0x01, 0x02}},
    {'g', {0x18, 0xa4, 0xa4, 0xa4, 0x7c}},
    {'h', {0x7f, 0x08, 0x04, 0x04, 0x78}},
    {'i', {0x00, 0x44, 0x7d, 0x40, 0x00}},
    {'j', {0x40, 0x80, 0x84, 0x7d, 0x00}},
    {'k', {0x7f, 0x10, 0x28, 0x44, 0x00}},
    {'l', {0x00, 0x41, 0x7f, 0x40, 0x00}},
    {'m', {0x7c, 0x04, 0x18, 0x04, 0x78}},
    {'n', {0x7c, 0x08, 0x04, 0x04, 0x78}},
    {'o', {0x38, 0x44, 0x44, 0x44, 0x38}},
    {'p', {0xfc, 0x24, 0x24, 0x24, 0x18}},
    {'q', {0x18, 0x24, 0x24, 0x18, 0xfc}},
    {'r', {0x7c, 0x08, 0x04, 0x04, 0x08}},
    {'s', {0x48, 0x54, 0x54, 0x54, 0x24}},
    {'t', {0x04, 0x3f, 0x44, 0x40, 0x20}},
    {'u', {0x3c, 0x40, 0x40, 0x20, 0x7c}},
    {'v', {0x1c, 0x20, 0x40, 0x20, 0x1c}},
    {'w', {0x3c, 0x40, 0x30, 0x40, 0x3c}},
    {'x', {0x44, 0x28, 0x10, 0x28, 0x44}},
    {'y', {0x1c, 0xa0, 0xa0, 0xa0, 0x7c}},
    {'z', {0x44, 0x64, 0x54, 0x4c, 0x44}},
    {'{', {0x00, 0x00, 0x40, 0x00, 0x00}}, // Wi-Fi very weak
    {'|', {0x00, 0x20, 0x50, 0x20, 0x00}}, // Wi-Fi weak
    {'}', {0x10, 0x28, 0x54, 0x28, 0x10}}, // Wi-Fi medium
    {'[', {0x1f, 0x11, 0x11, 0x11, 0x1f}}, // (MQTT Disconnected)
    {']',{0x1f, 0x11, 0x15, 0x11, 0x1f}}, // (MQTT Connecting / Subscribing)
    {'-', {0x08, 0x08, 0x08, 0x08, 0x08}},
};

static void clear_buffer(void) {
    memset(&oled_full_buffer[1], 0, OLED_SIZE);
    oled_full_buffer[0] = I2C_COMMAND_DATA_BYTE;
}

static const uint8_t* get_bitmap(char c) {
    for (int i = 0; i < sizeof(font5x7) / sizeof(font5x7[0]); i++) {
        if (font5x7[i].ascii == c) {
            return font5x7[i].bit_map;
        }
    }
    return font5x7[0].bit_map;
}

/* function to write char on screen first char second index */
static void write_char(char c, uint32_t start_idx)
{
    const uint8_t* bit_map = get_bitmap(c);
    if(start_idx != 0) oled_full_buffer_idx = start_idx;
    for(uint32_t i = 0; i < 5; ++i)
    {
        if(oled_full_buffer_idx > OLED_SIZE) oled_full_buffer_idx = 1;
        oled_full_buffer[oled_full_buffer_idx++] = bit_map[i];
    }
}

/* function to write string on screen */
static void write_string(char* c, uint32_t len, bool x_center, bool y_center, uint32_t x, uint32_t page)
{
    uint32_t start_idx = 1;
    if(x_center == true)
    {
        start_idx = (OLED_COLS - (5 * len)) / 2;
    }
    if(y_center == true)
    {
        uint32_t page_len = 1 + ((5 * len) / OLED_COLS);
        start_idx += ((OLED_PAGE - page_len) / 2 + 1) * OLED_COLS;
    }

    start_idx += x + page * OLED_COLS;

    write_char(c[0], start_idx);
    for(uint32_t i = 1; i < len; ++i)
    {
        write_char(c[i], 0);
    }
}

void write_distance(void)
{
    static float distance_cm;
    static char distance_lable[] = "dist";
    static char distance_char[10] = {};

    /* get distance value */
    xQueueReceive(q_distance, &distance_cm, 0);

    /* write app version and distance for object */
    snprintf(distance_char, sizeof(distance_char), "%.2f", distance_cm);

    write_string(distance_lable, strlen(distance_lable), false, false, 0, 3);
    write_string(distance_char, strlen(distance_char), false, true, 0, 0);
}

void write_cord(void)
{
    static char angle_time_char[64] = {};

    xQueueReceive(q_angle, angle_time_char, 0);

    write_string(angle_time_char, strlen(angle_time_char), false, true, 30, 0);
}

/* Function for write ota update percent and scale */
void write_ota_update(void)
{
    static uint32_t ota_percent_uint = 0;
    static uint32_t ota_scale_offset_idx = 7 * OLED_COLS + 10;

    static char ota_update[] = "OTA update";
    static char ota_percent_buf[4] = {};
    
    /* write ota update and percent to finish */
    write_string(ota_update, strlen(ota_update), true, false, 0, 6);

    /* get percent from ota task */
    xQueueReceive(q_ota_level, &ota_percent_uint, pdMS_TO_TICKS(10));

    /* write this data in the buffer*/
    snprintf(ota_percent_buf, sizeof(ota_percent_buf), "%ld%%", ota_percent_uint);
    write_string(ota_percent_buf, strlen(ota_percent_buf), false, false, 110, 7);

    /* write ota percent scale in to the buffer */
    for(uint32_t i = 0; i < ota_percent_uint; ++i)
    {
        oled_full_buffer[ota_scale_offset_idx + i] = 0xff;
    }
}

/* Function for write time and date */
void write_time(void)
{
    static struct tm timeinfo;
    static char time_buf[64] = {0};
    static time_t local_now;

    portENTER_CRITICAL(&m_TIME);
    local_now = now;
    portEXIT_CRITICAL(&m_TIME);

    /* set data into the buffer */
    localtime_r(&local_now, &timeinfo);

    snprintf(time_buf, sizeof(time_buf), "%d.%d.%d %d:%d:%d",   timeinfo.tm_year + 1900, 
                                                                timeinfo.tm_mon + 1,
                                                                timeinfo.tm_mday,
                                                                timeinfo.tm_hour,
                                                                timeinfo.tm_min,
                                                                timeinfo.tm_sec);

    if(strlen(time_buf) > 0)
    {
        write_string(time_buf, strlen(time_buf), false, false, 0, 1);
    }
}

/* Main task for handling oled display*/
void oled_task(void* pvParameters)
{  
    EventBits_t bits;

    uint8_t reset_cursor[] = {0x00, 0x21, 0, 127, 0x22, 0, 7};
    while(1)
    {
        clear_buffer();

        write_string(version_on_screen, strlen(version_on_screen), false, false, 0, 0);
        write_distance();

        bits = xEventGroupGetBits(e_tasks);

        if(WIFI_BIT_GOT_IP & bits)
        {
            /* write wi-fi signal */
            write_string("{|}", 3, false, false, OLED_COLS - 15, 0);
        }

        if(OTA_BIT_IN_PROGRESS & bits)
        {
            write_ota_update();
        }

        if(MQTT_SUBSCRIBE & bits)
        {
            write_string("[", 1, false, false, OLED_COLS - 5, 1);
        }
        else 
        {
            write_string("]", 2, false, false, OLED_COLS - 5, 1);
        }

        write_time();

        write_cord();
        
        /* write all data in the buffer */
        xSemaphoreTake(m_I2C0, portMAX_DELAY);
        i2c_master_transmit(i2c_master_dev_handle, reset_cursor, sizeof(reset_cursor), I2C_TIMEOUT);
        i2c_master_transmit(i2c_master_dev_handle, oled_full_buffer, OLED_SIZE + 1, I2C_TIMEOUT);
        xSemaphoreGive(m_I2C0);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}