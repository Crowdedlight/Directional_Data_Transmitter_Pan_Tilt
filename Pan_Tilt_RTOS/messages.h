/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.:
*
* PROJECT....:
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 160512  FMA   Module created.
*
*****************************************************************************/

#ifndef PAN_TILT_RTOS_MESSAGES_H_
#define PAN_TILT_RTOS_MESSAGES_H_

/***************************** Include files *******************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tm4c123gh6pm.h"
#include "EMP/emp_type.h"
#include "EMP/glob_def.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "uart.h"
#include "pid.h"
/*****************************    Defines    *******************************/

/*****************************    Externs    *******************************/
extern xQueueHandle uart_rx_queue;
extern xQueueHandle uart_tx_queue;
extern xQueueHandle pid_pan_setp_queue;
extern xQueueHandle pid_tilt_setp_queue;


/*****************************   Functions   *******************************/


void messages_rx_task();


/****************************** End Of Module *******************************/


#endif /* PAN_TILT_RTOS_MESSAGES_H_ */
