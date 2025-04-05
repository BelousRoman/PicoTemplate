#include "pico/stdlib.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#include "pico/bootrom.h"

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

#ifndef TEMPLATE_STR
#define TEMPLATE_STR    "No template string provided"
#endif // TEMPLATE_STR

int main()
{
	stdio_init_all();
    
	puts(TEMPLATE_STR);

#ifndef PICO_FREERTOS_BUILD
	reset_usb_boot(0, 0);
#endif

	lv_init();

	return 0;
}
