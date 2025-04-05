#include "ili9341.h"

static struct MSP3223 *_display;

unsigned int pwm_slice;
unsigned int pwm_chan;
unsigned int pwm_freq;
unsigned char pwm_duty;
unsigned short pwm_level;

bool ili9341_init(struct MSP3223 *display) {
    _display = display;

    // set your desired inputs here
	pwm_freq = PWM_FREQ; // frequency we want to generate
	pwm_duty = PWM_DUTY; // duty cycle, in percent

    if ((_display->lcd_led % 2) == 0)
    {
        pwm_chan = PWM_CHAN_A;
    }
    else
    {
        pwm_chan = PWM_CHAN_B;
    }

	gpio_set_function(_display->lcd_led, GPIO_FUNC_PWM); // alloc GPIO to the PWM
	pwm_slice = pwm_gpio_to_slice_num(_display->lcd_led); // get PWM slice

	// set frequency
	// determine top given Hz - assumes free-running counter rather than phase-correct
	uint32_t sys_freq = clock_get_hz(clk_sys); // typically 125'000'000 Hz
	float divider = (sys_freq / 1000000);  // let's arbitrarily choose to run pwm clock at 1MHz
	pwm_set_clkdiv(pwm_slice, divider); // pwm clock should now be running at 1MHz
	uint32_t top = (1000000/pwm_freq - 1); // TOP is u16 has a max of 65535, being 65536 cycles
	pwm_set_wrap(pwm_slice, top);

	// set duty cycle
	pwm_level = (top+1) * pwm_duty / 100 -1; // calculate channel level from given duty cycle in %
	pwm_set_chan_level(pwm_slice, pwm_chan, pwm_level); 
	
	pwm_set_enabled(pwm_slice, true); // Enable PWM

    spi_init(_display->spi, SPI_FREQ);
    _display->spi_freq = spi_set_baudrate(_display->spi, SPI_FREQ);
    // spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // Assign SPI functions to the default SPI pins
    gpio_set_function (_display->spi_sck, GPIO_FUNC_SPI);
    gpio_set_function (_display->spi_tx, GPIO_FUNC_SPI);
    gpio_set_function (_display->spi_rx, GPIO_FUNC_SPI);

    gpio_init(_display->lcd_cs);
    gpio_set_dir(_display->lcd_cs, GPIO_OUT);
    gpio_put(_display->lcd_cs, CS_PASSIVE);

    gpio_init(_display->lcd_rst);
    gpio_set_dir(_display->lcd_rst, GPIO_OUT);
    gpio_put(_display->lcd_rst, RST_IDLE);

    gpio_init(_display->lcd_rs);
    gpio_set_dir(_display->lcd_rs, GPIO_OUT);
    gpio_put(_display->lcd_rs, RS_COMMAND);

    #ifdef PICO_FREERTOS_BUILD
    vTaskDelay(10);
    #else
    sleep_ms(10);
    #endif

    gpio_put(_display->lcd_rst, RST_RESET);
    #ifdef PICO_FREERTOS_BUILD
    vTaskDelay(10);
    #else
    sleep_ms(10);
    #endif

    gpio_put(_display->lcd_rst, RST_IDLE);

    ili9341_send_command(SOFTWARE_RST);
    #ifdef PICO_FREERTOS_BUILD
    vTaskDelay(100);
    #else
    sleep_ms(100);
    #endif

    ili9341_send_command(DISPLAY_OFF);

    ili9341_send_command_w_data(PWR_CTL_B, "\x00\xC1\x30", 3); // {0x00, 0xC1, 0x30}
    ili9341_send_command_w_data(PWR_ON_SEQ_CTL, "\x64\x03\x12\x81", 4); // {0x64, 0x03, 0x12, 0x81}
    ili9341_send_command_w_data(DRV_TIMING_CTL_A, "\x85\x00\x78", 3); // {0x85, 0x00, 0x78}
    ili9341_send_command_w_data(PWR_CTL_A, "\x39\x2C\x00\x34\x02", 5); // {0x39, 0x2C, 0x00, 0x34, 0x02}
    ili9341_send_command_w_data(PUMP_RATIO_CTL, "\x20", 1); // {0x20}
    ili9341_send_command_w_data(DRV_TIMING_CTL_B, "\x00\x00", 2); // {0x00, 0x00}
    ili9341_send_command_w_data(PWR_CTL_1, "\x13", 1); // {0x13}
    ili9341_send_command_w_data(PWR_CTL_2, "\x13", 1); // {0x13}
    ili9341_send_command_w_data(VCOM_CTL_1, "\x1C\x35", 2); // {0x1C, 0x35}
    ili9341_send_command_w_data(VCOM_CTL_2, "\xC8", 1); // {0xC8}
    ili9341_send_command(DISPLAY_INV_MODE_ON);
    ili9341_send_command_w_data(DISPLAY_FUNC_CTL, "\x0A\xA2", 2); // {0x0A, 0xA2}
    ili9341_send_command_w_data(PIXEL_FMT_SET, "\x55", 1); // {0x55}
    ili9341_send_command_w_data(IF_CTL, "\x01\x30", 2); // {0x01, 0x30}
    ili9341_send_command_w_data(FRAME_CTL_NORMAL, "\x00\x1B", 2); // {0x00, 0x1B}
    ili9341_send_command_w_data(ENABLE_3_GAMMA_CTL, "\x00", 1); // {0x00}
    ili9341_send_command_w_data(GAMMA_SET, "\x04", 1); // {0x04} // 4th mode proved to be better by personal opinion
    ili9341_send_command_w_data(POS_GAMMA_CORRECTION, "\x0F\x35\x31\x0B\x0E\x06\x49\xA7\x33\x07\x0F\x03\x0C\x0A\x00", 15); // {0x0F, 0x35, 0x31, 0x0B, 0x0E, 0x06, 0x49, 0xA7, 0x33, 0x07, 0x0F, 0x03, 0x0C, 0x0A, 0x00}
    ili9341_send_command_w_data(NEG_GAMMA_CORRECTION, "\x00\x0A\x0F\x04\x11\x08\x36\x58\x4D\x07\x10\x0C\x32\x34\x0F", 15); // {0x00, 0x0A, 0x0F, 0x04, 0x11, 0x08, 0x36, 0x58, 0x4D, 0x07, 0x10, 0x0C, 0x32, 0x34, 0x0F}
    ili9341_send_command(SLEEP_OUT);
    
    #ifdef PICO_FREERTOS_BUILD
    vTaskDelay(120);
    #else
    sleep_ms(120);
    #endif

    ili9341_send_command(DISPLAY_ON);
    ili9341_set_orientation(_display->orientation);

    return true;

    return true;
}

bool ili9341_set_orientation(int orientation)
{
    switch (orientation)
    {
    case TOP_LEFT_PORTRAIT:
    {
        _display->width = DISPLAY_DIM_B;
        _display->height = DISPLAY_DIM_A;
        ili9341_send_command_w_data(MEM_ACCESS_CTL, (uint8_t []){MADCTL_BGR_BIT(1)}, 1);
    }
        break;
    case TOP_LEFT_LANDSCAPE:
    {
        _display->width = DISPLAY_DIM_A;
        _display->height = DISPLAY_DIM_B;
        ili9341_send_command_w_data(MEM_ACCESS_CTL, (uint8_t []){MADCTL_BGR_BIT(1) | MADCTL_MV_BIT(1)}, 1);
    }
        break;
    case TOP_RIGHT_PORTRAIT:
    {
        _display->width = DISPLAY_DIM_B;
        _display->height = DISPLAY_DIM_A;
        ili9341_send_command_w_data(MEM_ACCESS_CTL, (uint8_t []){MADCTL_BGR_BIT(1) | MADCTL_MX_BIT(1)}, 1);
    }
        break;
    case TOP_RIGHT_LANDSCAPE:
    {
        _display->width = DISPLAY_DIM_A;
        _display->height = DISPLAY_DIM_B;
        ili9341_send_command_w_data(MEM_ACCESS_CTL, (uint8_t []){MADCTL_BGR_BIT(1) | MADCTL_MV_BIT(1) | MADCTL_MX_BIT(1)}, 1);
    }
        break;
    case BOTTOM_LEFT_PORTRAIT:
    {
        _display->width = DISPLAY_DIM_B;
        _display->height = DISPLAY_DIM_A;
        ili9341_send_command_w_data(MEM_ACCESS_CTL, (uint8_t []){MADCTL_BGR_BIT(1) | MADCTL_MY_BIT(1)}, 1);
    }
        break;
    case BOTTOM_LEFT_LANDSCAPE:
    {
        _display->width = DISPLAY_DIM_A;
        _display->height = DISPLAY_DIM_B;
        ili9341_send_command_w_data(MEM_ACCESS_CTL, (uint8_t []){MADCTL_BGR_BIT(1) | MADCTL_MV_BIT(1) | MADCTL_MY_BIT(1)}, 1);
    }
        break;
    case BOTTOM_RIGHT_PORTRAIT:
    {
        _display->width = DISPLAY_DIM_B;
        _display->height = DISPLAY_DIM_A;
        ili9341_send_command_w_data(MEM_ACCESS_CTL, (uint8_t []){MADCTL_BGR_BIT(1) | MADCTL_MX_BIT(1) | MADCTL_MY_BIT(1)}, 1);
    }
        break;
    case BOTTOM_RIGHT_LANDSCAPE:
    {
        _display->width = DISPLAY_DIM_A;
        _display->height = DISPLAY_DIM_B;
        ili9341_send_command_w_data(MEM_ACCESS_CTL, (uint8_t []){MADCTL_BGR_BIT(1) | MADCTL_MV_BIT(1) | MADCTL_MX_BIT(1) | MADCTL_MY_BIT(1)}, 1);
    }
        break;
    default:
        return false;
        break;
    }

    _display->orientation = orientation;

    return true;
}

uint16_t ili9341_serialize_rgb24(uint8_t red, uint8_t green, uint8_t blue)
{
    return (uint16_t)((red & 0b11111000) << 8) | ((green & 0b11111100) << 3) | ((blue & 0b11111000) >> 3);
}

// uint16_t ili9341_set_back_color(uint16_t color)
// {
//     return 0;
// }

// uint16_t ili9341_set_fore_color(uint16_t color)
// {
//     return 0;
// }

void ili9341_set_window(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
    ili9341_send_command_w_data(COL_ADDR_SET, (uint8_t []){(uint8_t)(xs>>8), (uint8_t)(0xFF&xs), (uint8_t)(xe>>8), (uint8_t)(0xFF&xe)}, 4); // Set X
    ili9341_send_command_w_data(PAGE_ADDR_SET, (uint8_t []){(uint8_t)(ys>>8), (uint8_t)(0xFF&ys), (uint8_t)(ye>>8), (uint8_t)(0xFF&ye)}, 4); // Set Y
}

void ili9341_send_command(uint8_t command)
{
    gpio_put(_display->lcd_rs, RS_COMMAND);
    gpio_put(_display->lcd_cs, CS_ACTIVE);

    spi_write_blocking(_display->spi, &command, 1);

    gpio_put(_display->lcd_cs, CS_PASSIVE);
}

void ili9341_send_command_w_data(uint8_t command, uint8_t *data, int len)
{
    gpio_put(_display->lcd_rs, RS_COMMAND);
    gpio_put(_display->lcd_cs, CS_ACTIVE);

    spi_write_blocking(_display->spi, &command, 1);

    gpio_put(_display->lcd_rs, RS_DATA);

    spi_write_blocking(_display->spi, data, len);

    gpio_put(_display->lcd_cs, CS_PASSIVE);
}

void ili9341_send_data(uint8_t *data, int len)
{
    gpio_put(_display->lcd_rs, RS_DATA);
    gpio_put(_display->lcd_cs, CS_ACTIVE);

    spi_write_blocking(_display->spi, data, len);

    gpio_put(_display->lcd_cs, CS_PASSIVE);
}
