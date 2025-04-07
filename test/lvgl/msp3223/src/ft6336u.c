#include "ft6336u.h"

static struct MSP3223 *_display;

#ifndef PICO_LVGL_BUILD
static void touch_callback(uint gpio, __unused uint32_t events);
#endif

bool ft6336_init(struct MSP3223 *display)
{
    _display = display;

    gpio_init(_display->ctp_rst);
    gpio_set_dir(_display->ctp_rst, GPIO_OUT);
    gpio_put(_display->ctp_rst, RST_IDLE);

    gpio_init(_display->ctp_int);
    gpio_pull_up(_display->ctp_int);

#ifndef PICO_LVGL_BUILD
    gpio_set_irq_enabled_with_callback(_display->ctp_int, GPIO_IRQ_EDGE_RISE, true, touch_callback); // GPIO_IRQ_LEVEL_LOW
#endif

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

#ifdef PICO_LVGL_BUILD
    ft6336_send_register_w_data(FT6336U_ID_G_MODE, "\x00", 1); // {0x00}
#else
    ft6336_send_register_w_data(FT6336U_ID_G_MODE, "\x01", 1); // {0x01}
#endif

#ifndef PICO_LVGL_BUILD
    _display->tp_x1 = 0;
    _display->tp_y1 = 0;
    _display->tp_x2 = 0;
    _display->tp_y2 = 0;
#endif

    printf("FT6336U_ID_G_THGROUP: %d, FT6336U_ID_G_PERIODACTIVE: %d\n", 
            ft6336_read_register(FT6336U_ID_G_PMODE), 
            ft6336_read_register(FT6336U_ID_G_PERIODACTIVE));
    // ft6336_send_register_w_data(FT6336U_ID_G_THGROUP, (uint8_t []){15}, 1);
    ft6336_send_register_w_data(FT6336U_ID_G_PERIODACTIVE, (uint8_t []){0x4}, 1);
    printf("FT6336U_ID_G_THGROUP: %d, FT6336U_ID_G_PERIODACTIVE: %d\n", 
        ft6336_read_register(FT6336U_ID_G_PMODE), 
        ft6336_read_register(FT6336U_ID_G_PERIODACTIVE));

    return true;
}

#ifndef PICO_LVGL_BUILD
void touch_callback(uint gpio, __unused uint32_t events)
{
    if (gpio == _display->ctp_int)
    {
        static uint8_t touches;

        touches = ft6336_read_register(FT6336U_TD_STATUS);

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
            ft6336_read_two_touches(&_display->tp_x1, &_display->tp_y1, &_display->tp_x2, &_display->tp_y2);

            printf("Read 2 touches at %d:%d and %d:%d\n", _display->tp_x1, _display->tp_y1, _display->tp_x2, _display->tp_y2);
        }
            break;
        default:
            printf("Read touches : %d\n", touches);
            break;
        }
    }

    return;
}
#endif

void ft6336_read_one_touch(uint16_t *x1, uint16_t *y1)
{
    *x1 = ((ft6336_read_register(FT6336U_P1_YH) & 0b00001111) << 8) | ft6336_read_register(FT6336U_P1_YL);
    *y1 = ((ft6336_read_register(FT6336U_P1_XH) & 0b00111111) << 8) | ft6336_read_register(FT6336U_P1_XL);

    switch (_display->orientation)
    {
    case TOP_LEFT_PORTRAIT:
    {
        uint16_t tmp = *x1;
        *x1 = *y1;
        *y1 = tmp;
    }
        break;
    case TOP_LEFT_LANDSCAPE:
    {
        // Do nothing
    }
        break;
    case TOP_RIGHT_PORTRAIT:
    {
        uint16_t tmp = *x1;
        *x1 = abs(_display->width - *y1);
        *y1 = tmp;
    }
        break;
    case TOP_RIGHT_LANDSCAPE:
    {
        *y1 = abs(_display->height - *y1);
    }
        break;
    case BOTTOM_LEFT_PORTRAIT:
    {
        uint16_t tmp = *x1;
        *x1 = *y1;
        *y1 = tmp;

        *y1 = abs(_display->height - *y1);
    }
        break;
    case BOTTOM_LEFT_LANDSCAPE:
    {
        *x1 = abs(_display->width - *x1);
    }
        break;
    case BOTTOM_RIGHT_PORTRAIT:
    {
        uint16_t tmp = *x1;
        *x1 = *y1;
        *y1 = tmp;

        *x1 = abs(_display->width - *x1);
        *y1 = abs(_display->height - *y1);
    }
        break;
    case BOTTOM_RIGHT_LANDSCAPE:
    {
        *x1 = abs(_display->width - *x1);
        *y1 = abs(_display->height - *y1);
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

    switch (_display->orientation)
    {
    case TOP_LEFT_PORTRAIT:
    {
        uint16_t tmp = *x1;
        *x1 = *y1;
        *y1 = tmp;

        tmp = *x2;
        *x2 = *y2;
        *y2 = tmp;
    }
        break;
    case TOP_LEFT_LANDSCAPE:
    {
        // Do nothing
    }
        break;
    case TOP_RIGHT_PORTRAIT:
    {
        uint16_t tmp = *x1;
        *x1 = abs(_display->width - *y1);
        *y1 = tmp;

        tmp = *x2;
        *x2 = abs(_display->width - *y2);
        *y2 = tmp;
    }
        break;
    case TOP_RIGHT_LANDSCAPE:
    {
        *y1 = abs(_display->height - *y1);

        *y2 = abs(_display->height - *y2);
    }
        break;
    case BOTTOM_LEFT_PORTRAIT:
    {
        uint16_t tmp = *x1;
        *x1 = *y1;
        *y1 = tmp;

        *y1 = abs(_display->height - *y1);

        tmp = *x2;
        *x2 = *y2;
        *y2 = tmp;

        *y2 = abs(_display->height - *y2);
    }
        break;
    case BOTTOM_LEFT_LANDSCAPE:
    {
        *x1 = abs(_display->width - *x1);

        *x2 = abs(_display->width - *x2);
    }
        break;
    case BOTTOM_RIGHT_PORTRAIT:
    {
        uint16_t tmp = *x1;
        *x1 = *y1;
        *y1 = tmp;

        *x1 = abs(_display->width - *x1);
        *y1 = abs(_display->height - *y1);

        tmp = *x2;
        *x2 = *y2;
        *y2 = tmp;

        *x2 = abs(_display->width - *x2);
        *y2 = abs(_display->height - *y2);
    }
        break;
    case BOTTOM_RIGHT_LANDSCAPE:
    {
        *x1 = abs(_display->width - *x1);
        *y1 = abs(_display->height - *y1);

        *x2 = abs(_display->width - *x2);
        *y2 = abs(_display->height - *y2);
    }
        break;
    default:
        break;
    }
}

void ft6336_send_register(uint8_t reg)
{
    i2c_write_blocking(_display->i2c, FT6336U_ADDR, &reg, 1, false);
}

void ft6336_send_register_w_data(uint8_t reg, uint8_t *arg, int len)
{
    size_t data_size = (sizeof(uint8_t)*len) + 1;
    uint8_t *data = pvPortMalloc(data_size);
    if (data == NULL)
        return;
    
    memset(data, 0, (sizeof(uint8_t)*len) + 1);
    data[0] = reg;
    strncat(&data[1], arg, len);

    i2c_write_blocking(_display->i2c, FT6336U_ADDR, data, data_size, false);

    vPortFree(data);
}

uint8_t ft6336_read_register(uint8_t reg)
{
    uint8_t rxdata = 0;
    i2c_write_blocking(_display->i2c, FT6336U_ADDR,&reg, 1, false);
    i2c_read_blocking(_display->i2c, FT6336U_ADDR, &rxdata, 1, false);
    return rxdata;
}
