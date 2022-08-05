/*
    FreeRTOS V8.2.3
    This program explores the use of queues for inter-task communication. A queue 
    can be used to send/receive messages to/from tasks. 
*/

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* RISCV includes */
#include "arch/syscalls.h"
#include "arch/clib.h"
#include <stdio.h>

/*-----------------------------------------------------------*/

/*
 * FreeRTOS hook for when malloc fails, enable in FreeRTOSConfig.
 */
void vApplicationMallocFailedHook( void );

/*
 * FreeRTOS hook for when freertos is idling, enable in FreeRTOSConfig.
 */
void vApplicationIdleHook( void );

/*
 * FreeRTOS hook for when a stackoverflow occurs, enable in FreeRTOSConfig.
 */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );

/*-----------------------------------------------------------*/
xQueueHandle Global_Queue_Handle = NULL;
void sender(void *p)
{
    uint32_t i = 0;
    while (1) {
        printf(" Sending %d to receiver..\r\n\n", i);
        if (!xQueueSend(
            Global_Queue_Handle, // Queue Handle
            &i, // Item to queue
            1000 // No. of ticks to wait
        )) {
            printf("Failed to send to queue.\n");
        }
        i += 1;
        vTaskDelay(pdMS_TO_TICKS(2000)); // 2000ms delay
    }
}

void receiver(void* p)
{
    while (1) {
        int rx_i = 0;
        if (xQueueReceive(Global_Queue_Handle, &rx_i, 1000)) {
            printf("\n Received %d.\r\n", rx_i);
        }
        else {
            printf("Failed to receive data from queue.\r\n"); // The receiver waits for 1000 ticks. If no data received, this message is displayed
        }
       
    }
}

int main(void)
{
    /* Creating the queue */
    Global_Queue_Handle = xQueueCreate(
        3, // Queue Length
        sizeof( uint32_t ) // Size of each item
        );
    
    /* Creating the tasks */
    xTaskCreate(sender,
                "Tx", // Task Name
                1024,   // Stack Size 
                NULL,   // Parameters to task 
                1,      // Priority
                NULL    // Task Handler (optional)
                );
    xTaskCreate(receiver,
        "Rx", // Task Name
        1024,   // Stack Size 
        NULL,   // Parameters to task 
        1,      // Priority
        NULL    // Task Handler (optional)
    );

    /* Starting the scheduler */
    vTaskStartScheduler();
    return 0;
}
/*-----------------------------------------------------------*/

/* See the description at the top of this file. */
/*-----------------------------------------------------------*/
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/
