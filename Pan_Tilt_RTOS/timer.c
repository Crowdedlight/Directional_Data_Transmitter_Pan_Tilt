/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: timer.c
*
* PROJECT....: PAN & TILT
*
* DESCRIPTION:
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 160511  SSJ   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "timer.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void timer0_setup()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Enable clock for Timer0 (Run Mode Clock Gating)
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;

	// Do a dummy read to insert a few cycles after enabling the peripheral.
	INT8S dummy = SYSCTL_RCGCTIMER_R; // There should be minimum 3 clock cycles

	// Disable Timer0
	TIMER0_CTL_R &= ~TIMER_CTL_TAEN;

	// Set up Timer0 for 32 bit mode
	TIMER0_CFG_R |= TIMER_CFG_32_BIT_TIMER;

	// Set Timer0 to count up
	TIMER0_TAMR_R |= TIMER_TAMR_TACDIR;

	// Set Timer0 to periodic mode
	// TIMER0_TAMR_R |= TIMER_TAMR_TAMR_PERIOD;

	// Set upper boundary for Timer0
	//TIMER0_TAILR_R = 0x10;
}

void timer0_clear()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	TIMER0_TAV_R = 0;
}

INT32U timer0_read()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	return TIMER0_TAV_R;
}

void timer0_start()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Enable Timer0
	TIMER0_CTL_R |= TIMER_CTL_TAEN;
}

void timer0_stop()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Disable Timer0
	TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
}



/****************************** End Of Module *******************************/

