
/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: Main.c
*
* PROJECT....:
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 160512  FMA   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/

//Standard Stuff
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "EMP/emp_type.h"
#include "EMP/glob_def.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "systick.c"

//Application Specific
#include "spi.h"
#include "uart.h"
#include "messages.h"
#include "status_led.h"
#include "pid.h"
#include "app_tasks.h"


/*****************************    Defines    *******************************/
#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3
#define PID_PRIO  4
#define INCLUDE_vTaskSuspend 1

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************    Queues     *******************************/

//Uart
extern xQueueHandle uart_rx_queue;
extern xQueueHandle uart_tx_queue;

//SPI
extern xQueueHandle spi_rx_queue;
extern xQueueHandle spi_tx_queue;

//PID
extern xQueueHandle pid_pan_pos_queue;
extern xQueueHandle pid_tilt_pos_queue;
extern xQueueHandle pid_pan_setp_queue;
extern xQueueHandle pid_tilt_setp_queue;
extern xQueueHandle pid_pan_duty_queue;
extern xQueueHandle pid_tilt_duty_queue;

/*****************************  Semaphores   *******************************/



/*****************************   Functions   *******************************/

static void setupHardware(void)
{
  // TODO: Put hardware configuration and initialisation in here

	  init_systick();
	  status_led_init();

	  //UART setup
	  uart0_init(38400, 8, 1, 0);

	  //SPI setup
	  spi_init();

  // Warning: If you do not initialize the hardware clock, the timings will be inaccurate

}

int main(void)
{
  portBASE_TYPE return_value =pdTRUE;

  setupHardware();

  /*********** Setup Queues ***********/

  //uart
  uart_rx_queue = xQueueCreate(50, sizeof(INT8U));
  uart_tx_queue = xQueueCreate(50, sizeof(INT8U));

  //SPI
  spi_rx_queue = xQueueCreate(50, sizeof(INT32U));
  spi_tx_queue = xQueueCreate(50, sizeof(INT16U));

  //PID
  pid_pan_pos_queue   = xQueueCreate( 5, sizeof( INT16U ) );
  pid_tilt_pos_queue  = xQueueCreate( 5, sizeof( INT16U ) );
  pid_pan_setp_queue  = xQueueCreate( 5, sizeof( INT16U ) );
  pid_tilt_setp_queue = xQueueCreate( 5, sizeof( INT16U ) );
  pid_pan_duty_queue  = xQueueCreate( 5, sizeof( INT8U ) );
  pid_tilt_duty_queue = xQueueCreate( 5, sizeof( INT8U ) );

  /********** Setup Semaphores **********/


  /************ Start Tasks *************/
  return_value &= xTaskCreate(uart_tx_task, (signed portCHAR * ) "uart Transmit", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
  //return_value &= xTaskCreate(spi_task, (signed portCHAR * ) "spi task", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
  return_value &= xTaskCreate(messages_rx_task, (signed portCHAR * ) "messages task", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL );
  return_value &= xTaskCreate(controller_task, (signed portCHAR * ) "PID Controller", USERTASK_STACK_SIZE, NULL, PID_PRIO, NULL );
  return_value &= xTaskCreate(communication_task, (signed portCHAR * ) "communication task", USERTASK_STACK_SIZE, NULL, HIGH_PRIO, NULL );

  return_value &= xTaskCreate(status_led_task, ( signed portCHAR * ) 	"Status LED", 	USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );

  // test if all tasks started sucessfully
  if (return_value != pdTRUE)
  {
    GPIO_PORTD_DATA_R &= 0xBF;  // Turn on status LED.
    while(1);  // cold not create one or more tasks.
  }

 // Start the scheduler.
  vTaskStartScheduler();

 // Will only get here if there was insufficient memory to create the idle task.

  return 1;
}
