#ifndef _MSP3223_ILI9341_H
#define _MSP3223_ILI9341_H

#include <stdlib.h>
#include "pico/stdlib.h"

#ifdef CYW43_WL_GPIO_LED_PIN
    #include "pico/cyw43_arch.h"
#endif

#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "hardware/clocks.h"

#ifdef PICO_FREERTOS_BUILD
    #include "FreeRTOS.h"
    #include "task.h"
#endif

#include "msp3223_utils.h"

#define DISPLAY_DIM_A                           320
#define DISPLAY_DIM_B                           240

#define DISPLAY_MAX_A                           (DISPLAY_DIM_A - 1)
#define DISPLAY_MAX_B                           (DISPLAY_DIM_B - 1)

#define DISPLAY_BIT_COUNT                       (DISPLAY_DIM_A * DISPLAY_DIM_B)
#define DISPLAY_BYTE_COUNT                      (DISPLAY_BIT_COUNT / 8)

#define MADCTL_MH_BIT(x)                        (x << 2)
#define MADCTL_BGR_BIT(x)                       (x << 3)
#define MADCTL_ML_BIT(x)                        (x << 4)
#define MADCTL_MV_BIT(x)                        (x << 5)
#define MADCTL_MX_BIT(x)                        (x << 6)
#define MADCTL_MY_BIT(x)                        (x << 7)

/* Level 1 Commands */
#define NO_OP                                   0x00 // No Operation
#define SOFTWARE_RST                            0x01 // Software Reset
#define RD_DISPLAY_ID_INFO                      0x04 // Read Display Identification Information
#define RD_DISPLAY_STATUS                       0x09 // Read Display Status
#define RD_DISPLAY_PWR_MODE                     0x0A // Read Display Power Mode
#define RD_DISPLAY_MADCTL                       0x0B // Read Display MADCTL
#define RD_DISPLAY_PIXEL_FMT                    0x0C // Read Display Pixel Format
#define RD_DISPLAY_IMAGE_FMT                    0x0D // Read Display Image Format
#define RD_DISPLAY_SIG_MODE                     0x0E // Read Display Signal Mode
#define RD_DISPLAY_SELF_DIAG_RES                0x0F // Read Display Self-Diagnostic Result
#define ENTER_SLEEP_MODE                        0x10 // Enter Sleep Mode
#define SLEEP_OUT                               0X11 // Sleep Out
#define PARTIAL_MODE_ON                         0x12 // Partial Mode ON
#define NORMAL_MODE_ON                          0x13 // Normal Display Mode ON
#define DISPLAY_INV_MODE_OFF                    0x20 // Display Inversion OFF
#define DISPLAY_INV_MODE_ON                     0x21 // Display Inversion ON
#define GAMMA_SET                               0x26 // Gamma Set
#define DISPLAY_OFF                             0x28 // Display OFF
#define DISPLAY_ON                              0x29 // Display ON
#define COL_ADDR_SET                            0x2A // Column Address Set 
#define PAGE_ADDR_SET                           0x2B // Page Address Set
#define MEM_WRITE                               0x2C // Memory Write 
#define COLOR_SET                               0x2D // Color Set
#define MEM_READ                                0x2E // Memory Read
#define PARTIAL_AREA                            0x30 // Partial Area
#define VERT_SCROLL_DEF                         0x33 // Vertical Scrolling Definition
#define TEAR_EFF_LINE_OFF                       0x34 // Tearing Effect Line OFF
#define TEAR_EFF_LINE_ON                        0x35 // Tearing Effect Line ON
#define MEM_ACCESS_CTL                          0x36 // Memory Access Control
#define VERT_SCROLL_START_ADDR                  0x37 // Vertical Scrolling Start Address
#define IDLE_MODE_OFF                           0x38 // Idle Mode OFF
#define IDLE_MODE_ON                            0x39 // Idle Mode ON
#define PIXEL_FMT_SET                           0x3A // COLMOD: Pixel Format Set
#define WR_MEM_CONT                             0x3C // Write_Memory_Continue
#define RD_MEM_CONT                             0x3E // Read_Memory_Continue
#define SET_TEAR_SCANLINE                       0x44 // Set_Tear_Scanline
#define GET_SCANLINE                            0x45 // Get_Scanline
#define WR_DISPLAY_BRIGHTNESS                   0x51 // Write Display Brightness
#define RD_DISPLAY_BRIGHTNESS                   0x52 // Read Display Brightness
#define WR_CTRL_DISPLAY                         0x53 // Write CTRL Display
#define RD_CTRL_DISPLAY                         0x54 // Read CTRL Display
#define WR_CABC                                 0x55 // Write Content Adaptive Brightness Control
#define RD_CABC                                 0x56 // Read Content Adaptive Brightness Control
#define WR_CABC_MIN_BRIGHTNESS                  0x5E // Write CABC Minimum Brightness
#define RD_CABC_MIN_BRIGHTNESS                  0x5F // Read CABC Minimum Brightness
#define RD_ID1                                  0xDA // Read ID1
#define RD_ID2                                  0xDB // Read ID2
#define RD_ID3                                  0xDC // Read ID3

/* Level 2 Commands */
#define RGB_IF_SIG_CTL                          0xB0 // RGB Interface Signal Control
#define FRAME_CTL_NORMAL                        0xB1 // Frame Rate Control (In Normal Mode/Full Colors)
#define FRAME_CTL_IDLE                          0xB2 // Frame Rate Control (In Idle Mode/8 colors)
#define FRAME_CTL_PARTIAL                       0xB3 // Frame Rate control (In Partial Mode/Full Colors)
#define DISPLAY_INV_CTL                         0xB4 // Display Inversion Control
#define BLANKING_PORCH_CTL                      0xB5 // Blanking Porch Control
#define DISPLAY_FUNC_CTL                        0xB6 // Display Function Control
#define ENTRY_MODE_SET                          0xB7 // Entry Mode Set
#define BACKLIGHT_CTL_1                         0xB8 // Backlight Control 1
#define BACKLIGHT_CTL_2                         0xB9 // Backlight Control 2
#define BACKLIGHT_CTL_3                         0xBA // Backlight Control 3
#define BACKLIGHT_CTL_4                         0xBB // Backlight Control 4
#define BACKLIGHT_CTL_5                         0xBC // Backlight Control 5
#define BACKLIGHT_CTL_6                         0xBD // Backlight Control 6
#define BACKLIGHT_CTL_7                         0xBE // Backlight Control 7
#define BACKLIGHT_CTL_8                         0xBF // Backlight Control 8
#define PWR_CTL_1                               0xC0 // Power Control 1
#define PWR_CTL_2                               0xC1 // Power Control 2
#define VCOM_CTL_1                              0xC5 // VCOM Control 1
#define VCOM_CTL_2                              0xC7 // VCOM Control 2
#define NV_MEM_WR                               0xD0 // NV Memory Write
#define NV_MEM_PROT_KEY                         0xD1 // NV Memory Protection Key
#define NV_MEM_STATUS_RD                        0xD2 // NV Memory Status Read
#define READ_ID4                                0xD3 // Read ID4
#define POS_GAMMA_CORRECTION                    0xE0 // Positive Gamma Correction
#define NEG_GAMMA_CORRECTION                    0xE1 // Negative Gamma Correction
#define DIG_GAMMA_CTL_1                         0xE2 // Digital Gamma Control 1
#define DIG_GAMMA_CTL_2                         0xE3 // Digital Gamma Control 2
#define IF_CTL                                  0xF6 // Interface Control

/* Extend register command*/
#define PWR_CTL_A                               0xCB // Power control A
#define PWR_CTL_B                               0xCF // Power control B
#define DRV_TIMING_CTL_A                        0xE8 // Driver timing control A
#define DRV_TIMING_CTL_B                        0xEA // Driver timing control B
#define PWR_ON_SEQ_CTL                          0xED // Power on sequence control
#define ENABLE_3_GAMMA_CTL                      0xF2 // Enable 3 gamma
#define PUMP_RATIO_CTL                          0xF7 // Pump ratio control

bool ili9341_init(struct MSP3223 *);
bool ili9341_set_orientation(int orientation);
uint16_t ili9341_serialize_rgb24(uint8_t red, uint8_t green, uint8_t blue);
uint16_t ili9341_set_back_color(uint16_t color);
uint16_t ili9341_set_fore_color(uint16_t color);
void ili9341_set_window(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye);

void ili9341_send_command(uint8_t command);
void ili9341_send_command_w_data(uint8_t command, uint8_t *data, int len);
void ili9341_send_data(uint8_t *data, int len);

#endif /* _MSP3223_ILI9341_H */