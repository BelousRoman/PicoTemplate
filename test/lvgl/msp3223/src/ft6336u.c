#include "ft6336u.h"

static struct MSP3223 *_display;

static void gpio_callback(uint gpio, __unused uint32_t events);

bool ft6336_init(struct MSP3223 *display)
{
    _display = display;

    gpio_init(_display->ctp_rst);
    gpio_set_dir(_display->ctp_rst, GPIO_OUT);
    gpio_put(_display->ctp_rst, RST_IDLE);

    gpio_init(_display->ctp_int);
    // gpio_pull_up(_display->ctp_int);
    gpio_set_pulls(_display->ctp_int, false, true);
    // gpio_set_irq_enabled_with_callback(_display->ctp_int, GPIO_IRQ_LEVEL_LOW, true, gpio_callback);

    _display->i2c_freq = i2c_init(_display->i2c, display->i2c_freq);
    // i2c_set_slave_mode(i2c, false, 0);
    gpio_pull_up(_display->ctp_scl);
    gpio_pull_up(_display->ctp_sda);
    gpio_set_function(_display->ctp_scl, GPIO_FUNC_I2C);
    gpio_set_function(_display->ctp_sda, GPIO_FUNC_I2C);
    

    gpio_put(_display->ctp_rst, RST_RESET);
    vTaskDelay(20);
    gpio_put(_display->ctp_rst, RST_IDLE);
    vTaskDelay(500);

    ft6336_send_register_w_data(FT6336U_ID_G_MODE, "\x01", 1); // {0x01}

    display->tp_x1 = 0;
    display->tp_y1 = 0;
    display->tp_x2 = 0;
    display->tp_y2 = 0;

    return true;
}

void gpio_callback(uint gpio, __unused uint32_t events)
{
    puts("GPIO Callback");
    uint8_t touches = ft6336_read_register(FT6336U_TD_STATUS);

    switch (touches)
    {
    case 1:
    {
        ft6336_read_one_touch(&_display->tp_x1, &_display->tp_y1);

        printf("Read 1 touch at %d:%d\n", _display->tp_x1, _display->tp_y1);
    }
        break;
    case 2:
    {
        _display->tp_x1 = ((ft6336_read_register(FT6336U_P1_XH) & 0b00111111) << 8) | ft6336_read_register(FT6336U_P1_XL);
        _display->tp_y1 = ((ft6336_read_register(FT6336U_P1_YH) & 0b00001111) << 8) | ft6336_read_register(FT6336U_P1_YL);
        
        _display->tp_x2 = ((ft6336_read_register(FT6336U_P2_XH) & 0b00111111) << 8) | ft6336_read_register(FT6336U_P2_XL);
        _display->tp_y2 = ((ft6336_read_register(FT6336U_P2_YH) & 0b00001111) << 8) | ft6336_read_register(FT6336U_P2_YL);

        printf("Read 2 touches at %d:%d and %d:%d\n", _display->tp_x1, _display->tp_y1, _display->tp_x2, _display->tp_y2);
    }
        break;
    default:
        puts("Read no touches :(");
        break;
    }

    return;
}

void ft6336_read_one_touch(uint16_t *x1, uint16_t *y1)
{
    *x1 = ((ft6336_read_register(FT6336U_P1_YH) & 0b00001111) << 8) | ft6336_read_register(FT6336U_P1_YL);
    *y1 = ((ft6336_read_register(FT6336U_P1_XH) & 0b00111111) << 8) | ft6336_read_register(FT6336U_P1_XL);

    switch (_display->orientation)
    {
    case TOP_LEFT_PORTRAIT:
    {
        // Do nothing
    }
        break;
    case TOP_LEFT_LANDSCAPE:
    {
        uint16_t tmp = *x1;
        *x1 = *y1;
        *y1 = tmp;
    }
        break;
    case TOP_RIGHT_PORTRAIT:
    {
        *x1 = abs(_display->width - *x1);
    }
        break;
    case TOP_RIGHT_LANDSCAPE:
    {
        uint16_t tmp = *x1;
        *x1 = *y1;
        *y1 = tmp;

        *y1 = abs(_display->height - *y1);
    }
        break;
    case BOTTOM_LEFT_PORTRAIT:
    {
        *y1 = abs(_display->height - *y1);
    }
        break;
    case BOTTOM_LEFT_LANDSCAPE:
    {
        uint16_t tmp = *x1;
        *x1 = *y1;
        *y1 = tmp;

        *y1 = abs(_display->height - *y1);
    }
        break;
    case BOTTOM_RIGHT_PORTRAIT:
    {
        *x1 = abs(_display->width - *x1);
        *y1 = abs(_display->height - *y1);
    }
        break;
    case BOTTOM_RIGHT_LANDSCAPE:
    {
        uint16_t tmp = *x1;
        *x1 = *y1;
        *y1 = tmp;

        *x1 = abs(_display->width - *x1);
    }
        break;
    default:
        break;
    }
}

void ft6336_read_two_touches(uint16_t *x1, uint16_t *y1, uint16_t *x2, uint16_t *y2)
{
    *x1 = ((ft6336_read_register(FT6336U_P1_XH) & 0b00111111) << 8) | ft6336_read_register(FT6336U_P1_XL);
    *y1 = ((ft6336_read_register(FT6336U_P1_YH) & 0b00001111) << 8) | ft6336_read_register(FT6336U_P1_YL);
        
    *x2 = ((ft6336_read_register(FT6336U_P2_XH) & 0b00111111) << 8) | ft6336_read_register(FT6336U_P2_XL);
    *y2 = ((ft6336_read_register(FT6336U_P2_YH) & 0b00001111) << 8) | ft6336_read_register(FT6336U_P2_YL);
}

void ft6336_send_register(uint8_t reg)
{
    i2c_write_blocking(_display->i2c, FT6336U_ADDR, &reg, 1, false);
}

void ft6336_send_register_w_data(uint8_t reg, uint8_t *arg, int len)
{
    i2c_write_blocking(_display->i2c, FT6336U_ADDR, &reg, 1, true);
    i2c_write_blocking(_display->i2c, FT6336U_ADDR, arg, len, false);
}

uint8_t ft6336_read_register(uint8_t reg)
{
    uint8_t rxdata = 0;
    i2c_write_blocking(_display->i2c, FT6336U_ADDR,&reg, 1, false);
    i2c_read_blocking(_display->i2c, FT6336U_ADDR, &rxdata, 1, false);
    return rxdata;
}
