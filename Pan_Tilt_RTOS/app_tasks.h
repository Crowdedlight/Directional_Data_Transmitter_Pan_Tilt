/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: app_tasks.h
*
* PROJECT....: PAN TILT
*
* DESCRIPTION:
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 160515  FMA   Module created.
*
*****************************************************************************/

#ifndef APP_TASKS_H_
#define APP_TASKS_H_

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "EMP/emp_type.h"
#include "EMP/glob_def.h"
#include "timer.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "uart.h"
#include "spi.h"


/*****************************    Defines    *******************************/

/*****************************    Externs    *******************************/
extern xQueueHandle spi_rx_queue;
extern xQueueHandle spi_tx_queue;
extern xQueueHandle pid_pan_pos_queue;
extern xQueueHandle pid_tilt_pos_queue;
extern xQueueHandle pid_pan_duty_queue;
extern xQueueHandle pid_tilt_duty_queue;

//Debug
extern xQueueHandle uart_tx_queue;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void communication_task();
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : runs the pid controller x times a second and sends messages though SPI
******************************************************************************/


/****************************** End Of Module *******************************/

#endif /* APP_TASKS_H_ */
