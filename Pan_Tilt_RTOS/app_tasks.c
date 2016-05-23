/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: app_tasks.c
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

/***************************** Include files *******************************/
#include "app_tasks.h"

/*****************************    Defines    *******************************/

#define RUNPERSEC 200

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/


void communication_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	/***** Initialize *****/
	INT16U last_pan_value = 500;
	INT16U last_tilt_value = 500;
	INT16U new_pan_value = 500;
	INT16U new_tilt_value = 500;
	INT32U spiFeedback;
	INT16U spiTemp;

	/***** superloop *****/
	while(1)
	{
		xQueueReceive(pid_pan_duty_queue, &new_pan_value, 0);
		xQueueReceive(pid_tilt_duty_queue, &new_tilt_value, 0);

		//Check and update output if needed
		if (new_pan_value != last_pan_value)
		{
			//Send new spi value
			last_pan_value = new_pan_value;
		}

		if (new_tilt_value != last_tilt_value)
		{
			//Send new spi value
			last_tilt_value = new_tilt_value;

		}

		//Send spi data directly though SPI module function
		spiFeedback = writeSPI(1, last_pan_value, 1, last_tilt_value);

		//Send to controller queues which activates PID calculations. First pan then tilt
		spiTemp = ( spiFeedback & 0b111111111111000000000000) >> 12;
		xQueueSendToBack(pid_pan_pos_queue, &spiTemp, portMAX_DELAY);


		spiTemp = spiFeedback & 0b111111111111;
		xQueueSendToBack(pid_tilt_pos_queue, &spiTemp, portMAX_DELAY);

		//Now sleep in x times millisecs
		vTaskDelay((1000/RUNPERSEC) / portTICK_RATE_MS );

	}

}

/****************************** End Of Module *******************************/
