#include "pico/stdlib.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#include "pico/bootrom.h"
#include "hardware/spi.h"

#ifdef PICO_FREERTOS_BUILD
#include "FreeRTOS.h"
#include "task.h"
#endif

#ifdef PICO_FREERTOS_FAT_BUILD
#include "ff_headers.h"
#include "ff_sddisk.h"
#include "ff_stdio.h"
#include "ff_utils.h"

#include "hw_config.h"
#endif

#include "lvgl.h"

// #include "demos/stress/lv_demo_stress.h"
// #include "demos/lv_demos.h"
// #include "lv_demos.h"
#include "display.h"

#ifndef TEMPLATE_STR
#define TEMPLATE_STR    "No template string provided"
#endif // TEMPLATE_STR

struct MSP3223 msp3223 = {
#ifdef _PIO_SPI_H
	.spi.pio = pio0,
	.spi.sm = 0,
#else
	.spi = SPI_INST,
#endif
	.spi_freq = SPI_FREQ,
	.pwm_freq = PWM_FREQ,
	.pwm_duty = PWM_DUTY,
	.i2c = I2C_INST,
	.i2c_freq = I2C_FREQ,
	.spi_sck = SPI_SCK,
	.spi_tx = SPI_TX,
	.spi_rx = SPI_RX,
	.lcd_cs = LCD_CS,
	.lcd_rst = LCD_RST,
	.lcd_rs = LCD_RS,
	.lcd_led = LED_PWM,
	.sd_cs = SD_CS,
	.ctp_rst = CTP_RST,
	.ctp_int = CTP_INT,
	.ctp_sda = CTP_SDA,
	.ctp_scl = CTP_SCL,
	.orientation = BOTTOM_LEFT_LANDSCAPE
};

lv_draw_buf_t *buf1 = NULL;
lv_draw_buf_t *buf2 = NULL;

lv_obj_t *label1 = NULL;

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
	lv_obj_t *label = lv_event_get_user_data(e);

    /*Refresh the text*/
    lv_label_set_text_fmt(label, "%"LV_PRId32, lv_slider_get_value(slider));
    lv_obj_align_to(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);    /*Align top of the slider*/
}

void flush_screen_cb(lv_display_t *display, const lv_area_t *area, uint8_t *buf)
{
	ili9341_set_window(area->x1, area->y1, area->x2, area->y2);

	// ideally use by hardware
	lv_draw_sw_rgb565_swap(buf, DISPLAY_DIM_A * DISPLAY_DIM_B / 10);
	uint32_t len = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;

	ili9341_send_command_w_data(MEM_WRITE, buf, len);

    /* Inform LVGL that flushing is complete so buffer can be modified again. */
    lv_display_flush_ready(display);
}

void touch_read(lv_indev_t *indev, lv_indev_data_t *data)
{
	if(true == msp3223.tp_touch1) {
		disable_interrupts();

		data->point.x = msp3223.tp_x1;
		data->point.y = msp3223.tp_y1;

		msp3223.tp_touch1 = false;

		enable_interrupts();

		data->state = LV_INDEV_STATE_PRESSED;
		// printf("%s(1) - %d:%d\n", __func__, data->point.x, data->point.y);

		lv_label_set_text_fmt(label1, "%d;%d (x;y)", data->point.x, data->point.y);
    	lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);
	}
	else if (true == msp3223.tp_touch2) {
		disable_interrupts();

		data->point.x = msp3223.tp_x2;
		data->point.y = msp3223.tp_y2;

		msp3223.tp_touch2 = false;

		enable_interrupts();

		data->state = LV_INDEV_STATE_PRESSED;
		// printf("%s(2) - %d:%d\n", __func__, data->point.x, data->point.y);

		lv_label_set_text_fmt(label1, "%d;%d (x;y)", data->point.x, data->point.y);
    	lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);
	}
	else {
		data->state = LV_INDEV_STATE_RELEASED;
	}
}

void init_gui() {
	lv_obj_t *scr = lv_scr_act();  // Get the current screen
	lv_obj_set_style_bg_color(scr, lv_color_make(0xFF, 0xFF, 0xFF), LV_PART_MAIN);
	lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN); // Make it fully opaque

    lv_obj_t *btn1 = lv_btn_create(scr);
    lv_obj_set_size(btn1, 200, 50);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 0);

	label1 = lv_label_create(scr);
    lv_label_set_text(label1, "0:0");
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);

	lv_obj_t * slider = lv_slider_create(scr);
    lv_obj_set_width(slider, 200);
	lv_obj_align_to(slider, btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    lv_obj_t *label2 = lv_label_create(scr);
    lv_label_set_text(label2, "0");
    lv_obj_align_to(label2, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, label2);     /*Assign an event function*/
}

void lvgl_task(void *args) 
{
	display_init(&msp3223);

	lv_init();

	lv_tick_set_cb(xTaskGetTickCount); // or since_ms()

	/* Create a display */
	lv_display_t *display = lv_display_create(msp3223.width, msp3223.height);
	// lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
	// lv_disp_set_default(display);

    lv_display_set_flush_cb(display, flush_screen_cb);

	/* Set draw buffers for display */
    buf1 = lv_draw_buf_create(msp3223.width, (msp3223.height/10), LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    buf2 = lv_draw_buf_create(msp3223.width, (msp3223.height/10), LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    lv_display_set_draw_buffers(display, buf1, buf2);
    lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_obj_t *screen = lv_obj_create(NULL);

	/* Create input device connected to Default Display. */
	lv_indev_t *indev;

	indev = lv_indev_create();
	if (indev != NULL)
	{
		lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    	lv_indev_set_read_cb(indev, touch_read);
		// lv_indev_set_mode(index, LV_INDEV_MODE_EVENT);
		// lv_indev_set_display(indev, display);
	}
	else
	{
		puts("Failed to create Input Device!");
	}

	lv_obj_t * cursor_obj = lv_image_create(screen);  /* Create image Widget for cursor. */
	lv_image_set_src(cursor_obj, LV_SYMBOL_BULLET);             /* Set image source. */
	lv_indev_set_cursor(indev, cursor_obj);

	init_gui();

    while (1) {
        uint32_t time_till_next = lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(time_till_next));
    }

	vTaskDelete(NULL);
}

int main()
{
	stdio_init_all();
    
	puts(TEMPLATE_STR);

#ifndef PICO_FREERTOS_BUILD
	reset_usb_boot(0, 0);
#endif

    // Create the Hello World task
    xTaskCreate(lvgl_task, "lvgl_task", 16384, NULL, 1, NULL);    

    // Start FreeRTOS scheduler
    vTaskStartScheduler();

	panic("RTOS kernel is not running!"); // we shouldn't get here

	return 0;
}
