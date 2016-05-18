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

#define runPerSec 500

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/


void communication_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	/***** Initialize *****/
	INT8U last_pan_value = 50;
	INT8U last_tilt_value = 50;
	INT8U new_pan_value = 50;
	INT8U new_tilt_value = 50;
	INT32U spiFeedback;
	INT16U spiTemp;

	BOOLEAN once = FALSE;

	/***** superloop *****/
	while(1)
	{
		xQueueReceive(pid_pan_duty_queue, &new_pan_value, 10);
		xQueueReceive(pid_tilt_duty_queue, &new_tilt_value, 10);

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

			//Debug
			//xQueueSend(uart_tx_queue, &last_tilt_value, 10);
		}

		//Send spi data
		writeSPI(1, last_pan_value, 1, last_tilt_value);


		//Get feedback data and send to controller queues.
		xQueueReceive(spi_rx_queue, &spiFeedback, portMAX_DELAY);

		//Send to controller queues which activates PID calculations. First pan then tilt
		spiTemp = ( spiFeedback & 0b111111111111000000000000) >> 12;
		xQueueSendToBack(pid_pan_pos_queue, &spiTemp, 10);

		spiTemp = spiFeedback & 0b111111111111;
		xQueueSendToBack(pid_tilt_pos_queue, &spiTemp, 10);

		//Now sleep in x times millisecs
		vTaskDelay((1000/runPerSec) / portTICK_RATE_MS );

	}

}

/****************************** End Of Module *******************************/
