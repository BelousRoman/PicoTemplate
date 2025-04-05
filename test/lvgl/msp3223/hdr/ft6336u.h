#ifndef _MSP3223_FT6336U_H
#define _MSP3223_FT6336U_H

#include <stdlib.h>
#include "pico/stdlib.h"

#ifdef CYW43_WL_GPIO_LED_PIN
    #include "pico/cyw43_arch.h"
#endif

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"

#ifdef PICO_FREERTOS_BUILD
    #include "FreeRTOS.h"
    #include "task.h"
#endif

#include "msp3223_utils.h"

#define FT6336U_ADDR                        0x38

#define FT6336U_MODE_SWITCH                 0x00 // Register page (mode) switch
// 0X01 Reserved
#define FT6336U_TD_STATUS                   0x02 // Number of points reported
#define FT6336U_P1_XH                       0x03 // X coordinate of point 1 high 4 bit
#define FT6336U_P1_XL                       0X04 // X coordinate of point 1 low 8 bit
#define FT6336U_P1_YH                       0X05 // Y coordinate of point 1 high 4 bit
#define FT6336U_P1_YL                       0X06 // Y coordinate of point 1 low 8 bit
#define FT6336U_P1_WEIGHT                   0X07
#define FT6336U_P1_MISC                     0X08
#define FT6336U_P2_XH                       0X09 // X coordinate of point 2 high 4 bit
#define FT6336U_P2_XL                       0X0A // X coordinate of point 2 low 8 bit
#define FT6336U_P2_YH                       0X0B // Y coordinate of point 2 high 4 bit
#define FT6336U_P2_YL                       0X0C // Y coordinate of point 2 low 8 bit
#define FT6336U_P2_WEIGHT                   0X0D
#define FT6336U_P2_MISC                     0X0E
// 0x0F-0x7F Reserved
#define FT6336U_ID_G_THGROUP                0X80 // Touch threshold
// 0x81-0x84 Reserved
#define FT6336U_ID_G_THDIFF                 0X85 // Point filter range threshold
#define FT6336U_ID_G_CTRL                   0X86 // Monitor mode switch
#define FT6336U_ID_G_TIMEENTERMONITOR       0X87 // Enter the monitor delay without touching (in seconds)
#define FT6336U_ID_G_PERIODACTIVE           0X88 // Working Mode scan period
#define FT6336U_ID_G_PERIODMONITOR          0X89 // Monitor Mode scan cycle
// 0x8A Reserver
#define FT6336U_ID_G_FREQ_HOPPING_EN        0X8B // Charger identification
// 0x9C-0x95 Reserved
#define FT6336U_ID_G_TEST_MODE_FILTER       0X96 // Alpha filter enable in mass production test mode
// 0x97-0x9E Reserved
#define FT6336U_ID_G_CIPHER_MID             0X9F // Chip code (middle byte)
#define FT6336U_ID_G_CIPHER_LOW             0XA0 // Chip code (low byte)
#define FT6336U_ID_G_LIB_VERSION_H          0XA1 // High byte of APP library file version number
#define FT6336U_ID_G_LIB_VERSION_L          0XA2 // Low byte of APP library file version number
#define FT6336U_iD_G_CIPHER_HIGH            0XA3 // Chip code (high byte)
#define FT6336U_ID_G_MODE                   0XA4 // Report point INT mode
#define FT6336U_ID_G_PMODE                  0XA5 // Chip working mode
#define FT6336U_ID_G_FIRMID                 0XA6 // Firmware Verson
// 0xA7 Reserved
#define FT6336U_ID_G_FOCALTECH_ID           0XA8 // VENDOR ID
#define FT6336U_ID_G_VIRTUAL_KEY_THRES      0XA9 // Virtual button threshold in mass production test mode
// 0xAA-0xAC Reserved
#define FT6336U_ID_G_IS_CALLING             0XAD // Calling flag
#define FT6336U_ID_G_FACTORY_MODE           0XAE // Types of factory patterns
#define FT6336U_ID_G_RELEASE_CODE_ID        0XAF // Release code ID
#define FT6336U_ID_G_FACE_DEC_MODE          0XBO // Proximity sensing enable
// 0xB1-0xBB Reserved
#define FT6336U_ID_G_STATE                  0XBC // Work mode
// 0xBD-0xCF Reserved
#define FT6336U_ID_G_SPEC_GESTURE_ENABLE    0XD0 // Special gesture mode enable

bool ft6336_init(struct MSP3223 *);

void ft6336_read_one_touch(uint16_t *x1, uint16_t *y1);
void ft6336_read_two_touches(uint16_t *x1, uint16_t *y1, uint16_t *x2, uint16_t *y2);

void ft6336_send_register(uint8_t reg);
void ft6336_send_register_w_data(uint8_t reg, uint8_t *arg, int len);
uint8_t ft6336_read_register(uint8_t reg);

#endif /* _MSP3223_FT6336U_H */