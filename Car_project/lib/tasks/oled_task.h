#ifndef OLED_TASK_H
#define OLED_TASK_H

#include "stdio.h"
#include "string.h"
#include "esp_err.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "common.h"

#define I2C_DEFAULT_PORT I2C_NUM_0

#define I2C_SCL_PIN GPIO_NUM_8
#define I2C_SDA_PIN GPIO_NUM_9

#define I2C_SPEED_OLED 100000

#define I2C_SLAVE_ADDR 0x3C

#define I2C_SCL_WAIT_US 1000

#define I2C_TIMEOUT 200

/* COMANDS FOR OLED DISPLAY */
#define I2C_COMMAND_COMMAND_BYTE 0x00
#define I2C_COMMAND_DATA_BYTE 0x40

#define I2C_COMMAND_ChargePumpSetting 0x8D

#define I2C_COMMAND_SetContrastControlReset 0x7F
#define I2C_COMMAND_Enable_Charge_Pump 0x14

#define I2C_COMMAND_EntireDisplayOFFReset  0xA4
#define I2C_COMMAND_EntireDisplayON  0xA5

#define I2C_COMMAND_SetNormalDisplayReset  0xA6

#define I2C_COMMAND_SetDisplayON  0xAF

#define I2C_COMMAND_Set_Memory_Addressing_Mode 0x20
#define I2C_COMMAND_Set_Column_Address 0x21
#define I2C_COMMAND_Set_Page_Address 0x22

/* OLED SIZE */
#define OLED_SIZE 1024
#define OLED_PAGE 8
#define OLED_ROWS 64
#define OLED_COLS 128

typedef struct {
    char ascii;
    uint8_t bit_map[5];
}char_map_t ;

extern QueueHandle_t q_distance;

void set_version(const char* version);
void oled_init(void);
void write_distance(void);
void write_cord(void);
void write_ota_update(void);
void write_time(void);

void write_data(void);
void oled_task(void* pvParameters);

#endif  // DISTANCE_TASK_H