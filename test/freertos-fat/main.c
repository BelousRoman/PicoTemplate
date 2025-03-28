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

#define BUFFER_LEN      80

#define FS_NAME         "sd0"

void sd_task(void *params)
{
	printf("%s\n", pcTaskGetName(NULL));

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

    puts("End of Program");
    reset_usb_boot(0, 0);

    vTaskDelete(NULL);
}

int main()
{
	stdio_init_all();
    
	puts(TEMPLATE_STR);
	xTaskCreate(sd_task, "SD", 5000, NULL, (tskIDLE_PRIORITY+1), NULL);
	
	/* Start the tasks and timer running. */
	vTaskStartScheduler();
	
	reset_usb_boot(0, 0);
	return 0;
}

