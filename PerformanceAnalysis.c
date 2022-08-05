/*
    This is a program for measuring the performace parameters of FreeRTOS 
    on the SPIKE Simulator. 
    Processor Used: Generic 64-bit RISC-V processor
    Parameters Measured: 
        1) Task Creation Time - Stack size matters
        2) Task Deletion Time
        3) Context Switching Time
        4) Mutex Lock Time
        5) Mutex Unlock Time
        6) Boot Time
*/

/* Libraries included in the Kernel. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Common demo includes. */
#include "blocktim.h"
#include "countsem.h"
#include "recmutex.h"

/* For RISCV support */
#include "arch/syscalls.h"
#include "arch/clib.h"

/* The period after which the check timer will expire provided no errors have
been reported by any of the standard demo tasks.  ms are converted to the
equivalent in ticks using the portTICK_PERIOD_MS constant. */
#define mainCHECK_TIMER_PERIOD_MS			( 3000UL / portTICK_PERIOD_MS )

/* A block time of zero simply means "don't block". */
#define mainDONT_BLOCK						( 0UL )

/*-----------------------------------------------------------*/

/*
 * The check timer callback function, as described at the top of this file.
 */
static void prvCheckTimerCallback( TimerHandle_t xTimer );

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
TaskHandle_t vTask1Handle = NULL;   // Handler for Task 1
TaskHandle_t vTask2Handle = NULL;
TaskHandle_t vTask3Handle = NULL;
SemaphoreHandle_t xMutex;

void vTask1(void* p);
void vTask2(void* p);

long int before_contextswitch;
long int after_contextswitch;
long int contextswitch;

unsigned long read_cycles(void)
{
  unsigned long cycles;
  asm volatile ("rdcycle %0" : "=r" (cycles));
  return cycles;
}


void vTask1(void* p)
{
    while(1)
    {
        ;
    }
}

void vTask2(void* p)
{
    printf(" This is Task 2.\r\n\n");
    vTaskDelay(1000);
    // Task 2 executes first
    before_contextswitch = read_cycles(); // Start measuring the context switch
}

void vTask3(void* p)
{
    after_contextswitch = read_cycles();
    contextswitch = after_contextswitch - before_contextswitch;
    printf(" This is Task 3.\r\n\n");
    printf(" Context Switching Time: %ld\r\n\n", contextswitch);
    
    long int before_mutexlock = read_cycles();
    int val = xSemaphoreTake(xMutex, (TickType_t) 0xFFFFFFF);
    long int after_mutexlock = read_cycles();
    long int mutexlock = after_mutexlock - before_mutexlock;
    printf(" Mutex Lock Time: %ld\r\n\n", mutexlock);
    
    // Measuring the time taken to release the mutex
    long int before_mutexunlock = read_cycles();
    xSemaphoreGive(xMutex);
    long int after_mutexunlock = read_cycles();
    long int mutexunlock = after_mutexunlock - before_mutexunlock;
    printf(" Mutex Unlock Time: %ld\r\n\n", mutexunlock);

    vTaskDelay(pdMS_TO_TICKS(1000)); // 200ms delay

}

int main()
{
    printf("Boot Time: %ld\r\n", read_cycles()); // Reading the boot time

    // Calculating task creation time
    long int before_taskcreate = read_cycles(); 
    xTaskCreate(vTask1,                  // Function that the task executes
                "Task 1",               // Name
                1024,                   // Stack Size
                (void*)0,             // Parameters
                0,                   // Priority
                &vTask1Handle);     // Task Handler
    long int after_taskcreate = read_cycles(); 

    long int task_creation = after_taskcreate - before_taskcreate;
    printf(" Task Creation Time: %ld\r\n\n", task_creation);

    // Calculating task creation time
    long int before_taskdelete = read_cycles();
    vTaskDelete(vTask1Handle);
    long int after_taskdelete = read_cycles();

    long int taskdeletion = after_taskdelete - before_taskdelete;
    printf(" Task Deletion Time: %ld\r\n\n", taskdeletion);

    xTaskCreate(vTask2,                 // Function that the task executes
                "Task 2",              // Name
                1024,                  // Stack Size
                (void*)0,            // Parameters
                0,                  // Priority
                &vTask2Handle);    // Task Handler

     xTaskCreate(vTask3,                 // Function that the task executes
                "Task 3",              // Name
                1024,                  // Stack Size
                (void*)0,            // Parameters
                0,                  // Priority
                &vTask3Handle);    // Task Handler

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
static void prvCheckTimerCallback(__attribute__ ((unused)) TimerHandle_t xTimer )
{
static int count = 0;
unsigned long ulErrorFound = pdFALSE;

	/* Check all the demo and test tasks to ensure that they are all still
	running, and that none have detected an error. */

	if( xAreBlockTimeTestTasksStillRunning() != pdPASS )
	{
		printf("Error in block time test tasks \r\n");
		ulErrorFound |= ( 0x01UL << 1UL );
	}

	if( xAreCountingSemaphoreTasksStillRunning() != pdPASS )
	{
		printf("Error in counting semaphore tasks \r\n");
		ulErrorFound |= ( 0x01UL << 2UL );
	}

	if( xAreRecursiveMutexTasksStillRunning() != pdPASS )
	{
		printf("Error in recursive mutex tasks \r\n");
		ulErrorFound |= ( 0x01UL << 3UL );
	}

	if( ulErrorFound != pdFALSE )
	{
		__asm volatile("li t6, 0xbeefdead");
		printf("One or more threads has exited! \r\n");
	}else{
		__asm volatile("li t6, 0xdeadbeef");
		printf("[%d] All threads still alive! \r\n", count++);
	}

    /* Do _not_ stop the scheduler; this would halt the system, but was left for reference on how to do so */
	/* Stop scheduler */
//    vTaskEndScheduler();

}
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
