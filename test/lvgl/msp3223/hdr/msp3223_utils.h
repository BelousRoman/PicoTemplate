#ifndef _MSP3223_PINOUT_H
#define _MSP3223_PINOUT_H

#include "hardware/spi.h"
#include "hardware/i2c.h"

#define SPI_INST        spi0        /* default SPI instance used */
#define I2C_INST        i2c0        /* default SPI instance used */

#define SPI_FREQ        90000000    /* In HZ */
#define PWM_FREQ        1000        /* In HZ */
#define PWM_DUTY        50          /* In % */

#define I2C_FREQ        10000       /* In HZ*/

/* Set pins for MSP3223 display module */
#define SPI_SCK	        2
#define SPI_TX	        3
#define SPI_RX	        4

#define LCD_CS	        5
#define LCD_RST	        6
#define LCD_RS	        7
#define LED_PWM 	    8

#define SD_CS	        9

#define CTP_RST	        10
#define CTP_INT	        11
#define CTP_SDA	        12
#define CTP_SCL	        13

/* Control pins definitions */
#define CS_ACTIVE       0
#define CS_PASSIVE      1

#define RST_RESET       0
#define RST_IDLE        1

#define RS_COMMAND      0
#define RS_DATA         1

enum display_orientation {
    TOP_LEFT_PORTRAIT = 0,
    TOP_LEFT_LANDSCAPE,
    TOP_RIGHT_PORTRAIT,
    TOP_RIGHT_LANDSCAPE,
    BOTTOM_LEFT_PORTRAIT,
    BOTTOM_LEFT_LANDSCAPE,
    BOTTOM_RIGHT_PORTRAIT,
    BOTTOM_RIGHT_LANDSCAPE
};

struct MSP3223 {
    spi_inst_t *spi;
    uint32_t spi_freq;
    uint32_t pwm_freq;
    uint8_t pwm_duty;
    i2c_inst_t *i2c;
    uint32_t i2c_freq;
    uint8_t spi_sck;
    uint8_t spi_tx;
    uint8_t spi_rx;
    uint8_t lcd_cs;
    uint8_t lcd_rst;
    uint8_t lcd_rs;
    uint8_t lcd_led;
    uint8_t sd_cs;
    uint8_t ctp_scl;
    uint8_t ctp_sda;
    uint8_t ctp_int;
    uint8_t ctp_rst;
    uint8_t orientation;
    uint16_t width;
    uint16_t height;
    uint16_t tp_x1;
    uint16_t tp_y1;
    uint16_t tp_x2;
    uint16_t tp_y2;
};

#endif /* _MSP3223_PINOUT_H */