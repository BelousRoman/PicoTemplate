#include "pico/stdlib.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"

#include "pico/bootrom.h"

#include "FreeRTOS.h"
#include "task.h"

#include "ff_headers.h"
#include "ff_sddisk.h"
#include "ff_stdio.h"
#include "ff_utils.h"

#include "hw_config.h"

#ifndef TEMPLATE_STR
#define TEMPLATE_STR    "No template string provided"
#endif // TEMPLATE_STR

#ifndef BTN_DELAY_MS
#define BTN_DELAY_MS 50
#endif // BTN_DELAY_MS

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 500
#endif // LED_DELAY_MS

#define BUFFER_LEN      80

#define FS_NAME         "sd0"

// Perform initialisation
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}
    
// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}
    
bool __no_inline_not_in_flash_func(read_bootsel)()
{
    const uint CS_PIN_INDEX = 1;
    
    // Must disable interrupts, as interrupt handlers may be in flash, and we
    // are about to temporarily disable flash access!
    uint32_t flags = save_and_disable_interrupts();
    
    // Set chip select to Hi-Z
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);
    
    // Note we can't call into any sleep functions in flash right now
    for (volatile int i = 0; i < 1000; ++i);
    
    // The HI GPIO registers in SIO can observe and control the 6 QSPI pins.
    // Note the button pulls the pin *low* when pressed.
#if PICO_RP2040
    #define CS_BIT (1u << 1)
#else
    #define CS_BIT SIO_GPIO_HI_IN_QSPI_CSN_BITS
#endif
    bool button_state = !(sio_hw->gpio_hi_in & CS_BIT);
    
    // Need to restore the state of chip select, else we are going to have a
    // bad time when we return to code in flash!
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);
    
    restore_interrupts(flags);
    
    return button_state;
}

void sd_task(void *params)
{
	printf("TASK: %s\n", pcTaskGetName(NULL));

    FF_Disk_t *pxDisk = NULL;

    pxDisk = FF_SDDiskInit(FS_NAME);
    if(pxDisk != NULL)
    {
        printf("Init disk: SUCCESS\n");

        FF_Error_t ret = FF_SDDiskMount(pxDisk);
        if (FF_isERR(ret) == pdFALSE) {
            printf("Mount disk: SUCCESS\n");

            if (FF_FS_Add("/"FS_NAME, pxDisk) != 0)
            {
                if (ff_chdir(FS_NAME) != 0)
                    printf("ff_chdir failed: %s (%d)\n", strerror(stdioGET_ERRNO()),
                                                        stdioGET_ERRNO());

                char *buffer = (char *)pvPortMalloc(BUFFER_LEN);

                if (buffer != NULL) {
                    memset(buffer, 0, BUFFER_LEN);

                    if (ff_getcwd(buffer, BUFFER_LEN) != NULL)
                        printf("Current working directory is <%s>\n", buffer);

                    FF_FindData_t *pxFindStruct;
                    const char *pcAttrib, 
                                *pcWritableFile = "Writable file", 
                                *pcReadOnlyFile = "Read only file", 
                                *pcDirectory = "Directory";
                    
                    pxFindStruct = (FF_FindData_t *)pvPortMalloc(sizeof(FF_FindData_t));

                    /* FF_FindData_t must be cleared to 0. */
                    memset(pxFindStruct, 0, sizeof(FF_FindData_t));
                    
                    /* The first parameter to ff_findfist() is the directory being searched. Do
                        not add wildcards to the end of the directory name. */
                    if( ff_findfirst( buffer, pxFindStruct ) == 0 )
                    {
                        printf("Scanning directory %s\n", buffer);

                        do
                        {
                            /* Point pcAttrib to a string that describes the file. */
                            if( (pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR) != 0)
                            {
                                pcAttrib = pcDirectory;
                            }
                            else if(pxFindStruct->ucAttributes & FF_FAT_ATTR_READONLY)
                            {
                                pcAttrib = pcReadOnlyFile;
                            }
                            else
                            {
                                pcAttrib = pcWritableFile;
                            }
                    
                            /* Print the files name, size, and attribute string. */
                            printf("%s: \"%s\" (%d bytes)\n", pcAttrib,
                                                        pxFindStruct->pcFileName,
                                                        pxFindStruct->ulFileSize);
                    
                        } while(ff_findnext(pxFindStruct) == 0);
                    }
                    else
                    {
                        printf("Directory %s is empty\n", buffer);
                    }

                    vPortFree(buffer);
                }

                FF_FS_Remove("/"FS_NAME);
                printf("Removed filesystem\n");
            }

            FF_Unmount(pxDisk);

            printf("Unmounted disk\n");
        } else {
            printf("Mount disk: FAILURE\n"
                    "Error message: %s (%d)", (const char *)FF_GetErrMessage(ret), ret);
        }

        FF_SDDiskDelete(pxDisk);

        printf("Deleted disk\n");
    }
    else
        printf("Init disk: FAILURE\n");

    vTaskDelete(NULL);
}

void btn_task(void *params)
{
	printf("TASK: %s\n", pcTaskGetName(NULL));
	
	while (read_bootsel() != true)
    		vTaskDelay(BTN_DELAY_MS);

	puts("End of Program");
    reset_usb_boot(0, 0);
}

void led_task(void *params)
{
	printf("TASK: %s\n", pcTaskGetName(NULL));

	int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    while(1)
    {
        pico_set_led(true);
        vTaskDelay(LED_DELAY_MS);
        pico_set_led(false);
        vTaskDelay(LED_DELAY_MS);
    }

    vTaskDelete(NULL);
}

int main()
{
	stdio_init_all();
    
	puts(TEMPLATE_STR);

    xTaskCreate(btn_task, "BOOTSEL BUTTON", 5000, NULL, (tskIDLE_PRIORITY+1), NULL);
    xTaskCreate(led_task, "LED BLINK", 5000, NULL, (tskIDLE_PRIORITY+1), NULL);
	xTaskCreate(sd_task, "SD", 5000, NULL, (tskIDLE_PRIORITY+2), NULL);
	
	/* Start the tasks and timer running. */
	vTaskStartScheduler();
	
	reset_usb_boot(0, 0);
	return 0;
}

