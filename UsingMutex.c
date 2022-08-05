/*
    FreeRTOS V8.2.3

    This program explores the use of mutexes. Mutexes are mutual exclusion
    objects, which are used to allow task synchronisation when accessing
    the same resources. With the locking of a mutex, the task that locks the
    mutex is allowed to run until it releases the mutex. In this program,
    two tasks write two different messages onto a shared array with the help
    of a mutex.
*/

/*

 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* RISCV includes */
#include "arch/syscalls.h"
#include "arch/clib.h"

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
TaskHandle_t vTask1Handle = NULL;
TaskHandle_t vTask2Handle = NULL;
SemaphoreHandle_t xMutex; // Mutex Object
char myResource[10]; // Shared Resource

void vTask1(void* p);
void vTask2(void* p);


void vTask1(void* p)
{
    char Msg1[] = "Violet";
    int i;
    while(1)
    {
        if(xSemaphoreTake(xMutex, (TickType_t) 0xFFFFFFF)) // Lock the mutex
                                                           // for a large amount of time
        {
            for(i = 0; i < 9; i++){
                myResource[i] = Msg1[i];
                vTaskDelay(pdMS_TO_TICKS(100)); // 100ms delay
            }
            myResource[i] = 0;
            printf("Task 1: %s\r\n", myResource);
            xSemaphoreGive(xMutex); // Unlock the mutex
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // 200ms delay
    }
}

void vTask2(void* p)
{
    char Msg2[] = "Purple";
    int i;
    while(1)
    {
        if(xSemaphoreTake(xMutex, (TickType_t) 0xFFFFFFF))
        {
            for(i = 0; i < 9; i++){
                myResource[i] = Msg2[i];
                vTaskDelay(50);
            }
            myResource[i] = 0;
            printf("Task 2: %s\r\n", myResource);
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(100);
    }
}


int main(void)
{

    xTaskCreate(vTask1,             // Function that the task executes
                "Task 1",            // Name
                200,                // Stack Size
                (void*)0,                // Parameters
                0,                  // Priority
                &vTask1Handle);      // Task Handler

    xTaskCreate(vTask2,             // Function that the task executes
                "Task 2",            // Name
                200,                // Stack Size
                (void*)0,                // Parameters
                0,                  // Priority
                &vTask2Handle);      // Task Handler

    xMutex = xSemaphoreCreateMutex();
    vTaskStartScheduler();
    while(1)
    {
        ;
    }
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
