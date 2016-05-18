/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: spi.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150228  FMA   Module created.
*
*****************************************************************************/

#ifndef _SPI_H
  #define _SPI_H

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "EMP/emp_type.h"
#include "EMP/glob_def.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "uart.h"

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void spi_init();
INT32U writeSPI(int enable1, INT8U motor1, int enable2, INT8U motor2);
void spi_task();

/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

