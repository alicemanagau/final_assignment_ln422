#include <stdio.h> //Pour pouvoir coder en c (?)
#include <pthread.h> // Sert a faire du multitache et des threads en c

#include "FreeRTOS.h"
#include "task.h" //include in FreeRtos 
#include "timers.h"
#include "semphr.h"

/* Local includes. */
#include "console.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_SEND_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )

#define task1_TASK_PRIORITY				   (tskIDLE_PRIORITY + 6 )
#define task2_TASK_PRIORITY				   (tskIDLE_PRIORITY + 5 )
#define task3_TASK_PRIORITY				   (tskIDLE_PRIORITY + 4 )
#define task4_TASK_PRIORITY				   (tskIDLE_PRIORITY + 3 )

/* The rate at which data is sent to the queue.  The times are converted from
 * milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK_SEND_FREQUENCY_MS         pdMS_TO_TICKS( 200UL )
#define mainTIMER_SEND_FREQUENCY_MS        pdMS_TO_TICKS( 2000UL )
#define task1_PERIOD					   pdMS_TO_TICKS (500)
#define task2_PERIOD					   pdMS_TO_TICKS (1000)
#define task3_PERIOD					   pdMS_TO_TICKS (2500)
#define task4_PERIOD					   pdMS_TO_TICKS (5000)

/* The number of items the queue can hold at once. */
#define mainQUEUE_LENGTH                   ( 2 )

/* The values sent to the queue receive task from the queue send task and the
 * queue send software timer respectively. */
#define mainVALUE_SENT_FROM_TASK           ( 100UL )
#define mainVALUE_SENT_FROM_TIMER          ( 200UL )


/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void * pvParameters );
static void prvQueueSendTask( void * pvParameters );
static void task1(void * pvParameters); 
static void task2(void * pvParameters); 
static void task3(void * pvParameters); 
static void task4(void * pvParameters); 


/*
 * The callback function executed when the software timer expires.
 */
static void prvQueueSendTimerCallback( TimerHandle_t xTimerHandle );

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

/*-----------------------------------------------------------*/
/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/
void ipsa_sched( void )
{
    const TickType_t xTimerPeriod = mainTIMER_SEND_FREQUENCY_MS;

    /* Create the queue. */
    xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

    if( xQueue != NULL )
    {
        /* Start the two tasks as described in the comments at the top of this
         * file. */
        xTaskCreate( prvQueueReceiveTask,             /* The function that implements the task. */
                     "Rx",                            /* The text name assigned to the task - for debug only as it is not used by the kernel. */
                     configMINIMAL_STACK_SIZE,        /* The size of the stack to allocate to the task. */
                     NULL,                            /* The parameter passed to the task - not used in this simple case. */
                     mainQUEUE_RECEIVE_TASK_PRIORITY, /* The priority assigned to the task. */
                     NULL );                          /* The task handle is not required, so NULL is passed. */

        
		
		xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE, NULL,  task1_TASK_PRIORITY, NULL );
		xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE, NULL,  task2_TASK_PRIORITY, NULL );
		xTaskCreate(task3, "task3", configMINIMAL_STACK_SIZE, NULL,  task3_TASK_PRIORITY, NULL );
		xTaskCreate(task4, "task4", configMINIMAL_STACK_SIZE, NULL,  task4_TASK_PRIORITY, NULL );
		
        /* Create the software timer, but don't start it yet. */
        xTimer = xTimerCreate( "Timer",                     /* The text name assigned to the software timer - for debug only as it is not used by the kernel. */
                               xTimerPeriod,                /* The period of the software timer in ticks. */
                               pdTRUE,                      /* xAutoReload is set to pdTRUE. */
                               NULL,                        /* The timer's ID is not used. */
                               prvQueueSendTimerCallback ); /* The function executed when the timer expires. */

        if( xTimer != NULL )
        {
            xTimerStart( xTimer, 0 );
        }

        /* Start the tasks and timer running. */
        vTaskStartScheduler();
    }

    /* If all is well, the scheduler will now be running, and the following
     * line will never be reached.  If the following line does execute, then
     * there was insufficient FreeRTOS heap memory available for the idle and/or
     * timer tasks	to be created.  See the memory management section on the
     * FreeRTOS web site for more details. */
    for( ; ; )
    {
    }
}

/*-----------------------------------------------------------*/

static void prvQueueSendTask( void * pvParameters )
{
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainTASK_SEND_FREQUENCY_MS;
    const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TASK;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ; ; )
    {
        /* Place this task in the blocked state until it is time to run again.
        *  The block time is specified in ticks, pdMS_TO_TICKS() was used to
        *  convert a time specified in milliseconds into a time specified in ticks.
        *  While in the Blocked state this task will not consume any CPU time. */
        vTaskDelayUntil( &xNextWakeTime, xBlockTime );

        /* Send to the queue - causing the queue receive task to unblock and
         * write to the console.  0 is used as the block time so the send operation
         * will not block - it shouldn't need to block as the queue should always
         * have at least one space at this point in the code. */
        xQueueSend( xQueue, &ulValueToSend, 0U );
    }
}

/*-----------------------------------------------------------*/

static void prvQueueSendTimerCallback( TimerHandle_t xTimerHandle )
{
    const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TIMER;

    /* This is the software timer callback function.  The software timer has a
     * period of two seconds and is reset each time a key is pressed.  This
     * callback function will execute if the timer expires, which will only happen
     * if a key is not pressed for two seconds. */

    /* Avoid compiler warnings resulting from the unused parameter. */
    ( void ) xTimerHandle;

    /* Send to the queue - causing the queue receive task to unblock and
     * write out a message.  This function is called from the timer/daemon task, so
     * must not block.  Hence the block time is set to 0. */
    xQueueSend( xQueue, &ulValueToSend, 0U );
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void * pvParameters )
{
    uint32_t ulReceivedValue;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    for( ; ; )
    {
        /* Wait until something arrives in the queue - this task will block
         * indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
         * FreeRTOSConfig.h.  It will not use any CPU time while it is in the
         * Blocked state. */
        xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

        /* To get here something must have been received from the queue, but
         * is it an expected value?  Normally calling printf() from a task is not
         * a good idea.  Here there is lots of stack space and only one task is
         * using console IO so it is ok.  However, note the comments at the top of
         * this file about the risks of making Linux system calls (such as
         * console output) from a FreeRTOS task. */
        if( ulReceivedValue == mainVALUE_SENT_FROM_TASK )
        {
            console_print( "Message received from task\n" );
        }
        else if( ulReceivedValue == mainVALUE_SENT_FROM_TIMER )
        {
            console_print( "Message received from software timer\n" );
        }
        else
        {
            console_print( "Unexpected message\n" );
        }
    }
}
/*-----------------------------------------------------------*/

void task1(void * pvParameters) 
{
	TickType_t xNextWakeTime;
    const TickType_t xBlockTime = task1_PERIOD;
    const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TASK;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();
	
	
	
	for ( ; ; )
	{
		vTaskDelayUntil( &xNextWakeTime, xBlockTime );
		printf("Task 1 : Working");
		printf("\n");
		
	}
}


/*-----------------------------------------------------------*/

void task2(void * pvParameters) 
{
	TickType_t xNextWakeTime;
    const TickType_t xBlockTime = task2_PERIOD;
    const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TASK;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();
	
	float F = 75;
	float C;
	
	for ( ; ; )
	{
		vTaskDelayUntil( &xNextWakeTime, xBlockTime );
		C = (F - 32) / 1.8 ;
		printf("Task 2 : Convertion of %f (Fahrenheit) in %f (celsius)",F,C);
		printf("\n");
	}
}



/*-----------------------------------------------------------*/

void task3(void * pvParameters) 
{
	TickType_t xNextWakeTime;
    const TickType_t xBlockTime = task3_PERIOD;
    const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TASK;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();
	
	long int A=256723412;
	long int B=198767312;
	long int multiplication;
	
	for ( ; ; )
	{
		vTaskDelayUntil( &xNextWakeTime, xBlockTime );
		multiplication=A*B;
		printf("Task 3 : Multiplication of %ld with %ld equal to %ld",A,B,multiplication);
		printf("\n");
	}
}


/*-----------------------------------------------------------*/

void task4(void * pvParameters) 
{
	TickType_t xNextWakeTime;
    const TickType_t xBlockTime = task4_PERIOD;
    const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TASK;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();
	int tableau[]={3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52}; //tableau de 50 éléments
	int n = sizeof(tableau) / sizeof(tableau[0]);
	int x=26;
	int low=0;
	int high=n-1;
	int check=-1;
	int m,milieu;
	
	
	for ( ; ; )
	{
		vTaskDelayUntil( &xNextWakeTime, xBlockTime );
		// jusqua ce que le pointeur haut et bas se rejoignent 
		while (low <= high) 
		{
			int milieu = low + (high - low) / 2;
			if (tableau[milieu] == x)
				//milieu=milieu;
				m = milieu;
				check =0;

			if (tableau[milieu] < x)
				low = milieu + 1;

			else
				high = milieu - 1;
		}
		
		
	if ( check == -1)
		printf("Task 4 : La valeur n'est pas dans le tableau \n");
		
	else
		printf("Task 4 : la valeur est à l'occurence  %d \n", m);
	}
}


