/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: pid.h
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
* 160504  SSJ   Module created.
*
*****************************************************************************/

#ifndef _PID_H
  #define _PID_H

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "EMP/emp_type.h"
#include "EMP/glob_def.h"
#include "timer.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "uart.h"
//#include "semphr.h"

extern xQueueHandle uart_tx_queue;

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

// Input queues for position data from pan and tilt system
xQueueHandle pid_pan_pos_queue;
xQueueHandle pid_tilt_pos_queue;

// Input queues for desired set-points from user or tracking system
xQueueHandle pid_pan_setp_queue;
xQueueHandle pid_tilt_setp_queue;

// Output queues for duty cycles to be sent to pan and tilt system
xQueueHandle pid_pan_duty_queue;
xQueueHandle pid_tilt_duty_queue;

//DEBUG TODO REMOVE
INT16U pan_debug;
INT16U tilt_debug;
INT16S pan_err2;
INT16S tilt_err2;

/*****************************   Functions   *******************************/
INT16S pid_p_part( INT16S err );
/*****************************************************************************
*   Input    : Error signal
*   Output   : Controller proportional output
*   Function : Calculates the proportional part of the controller output
******************************************************************************/

INT16S pid_i_part( INT16S err, INT32S * int_err, INT16U dt );
/*****************************************************************************
*   Input    : Error signal, error integral, time passed since last sample
*   Output   : Controller integral output
*   Function : Calculates the integral part of the controller output
******************************************************************************/

INT16S pid_d_part( INT16S err, INT16S prev_err, INT16U dt );
/*****************************************************************************
*   Input    : Error signal, previous error signal, time passed since last sample
*   Output   : Controller derivative output
*   Function : Calculates the derivative part of the controller output
******************************************************************************/

INT8U pid_calculate_duty ( INT16S err, INT16S * int_err, INT16S prev_err, INT16U dt );
/*****************************************************************************
*   Input    : Error signal, error integral, previous error signal, time passed since last sample
*   Output   : Output signal as duty cycle
*   Function : Calculates the pid output
******************************************************************************/

INT8U pan_convert_to_duty( INT16S pid_out );
INT8U tilt_convert_to_duty( INT16S pid_out );
/*****************************************************************************
*   Input    : Positive/negative percentage of maximum output
*   Output   : Output in duty cycle from 0 - 100
*   Function : Converts output from percentage to duty cycle
******************************************************************************/

INT16U deg10_to_encoder_counts( INT16U deg10 );
/*****************************************************************************
*   Input    : Position in 10th of degrees
*   Output   : Position in encoder counts
*   Function : Converts position from 10th of degrees to encoder counts
******************************************************************************/

INT16U validate_pan_setp( INT16U setp );
/*****************************************************************************
*   Input    : Pan set-point
*   Output   : New pan set-point
*   Function : Makes sure pan set-point is between 540-200 and 540+200
******************************************************************************/

INT16U validate_tilt_setp( INT16U setp );
/*****************************************************************************
*   Input    : Tilt set-point
*   Output   : New tilt set-point
*   Function : Makes sure tilt set-point is between 0 and 1079
******************************************************************************/

INT16S check_tilt_err_direction( INT16S err );
/*****************************************************************************
*   Input    : Error signal
*   Output   : New error signal
*   Function : Checks if it is shorter to move past the 1079-0 limit
******************************************************************************/

void controller_task();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : -
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

